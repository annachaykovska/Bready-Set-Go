#include <iostream>

#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

#include "../SystemManager.h"
#include "RenderingSystem.h"
#include "../Scene/Scene.h"
#include "../Scene/Entity.h"
#include <stbi/stb_image.h>
#include "../Timer.h"
#include "UISystem.h"
#include "../Gameplay/GameLoopManager.h"
#include "../Profiler.h"

extern Scene g_scene;
extern SystemManager g_systems;

namespace
{
	const bool DEBUG_NAVMESH = false;
}

RenderingSystem::RenderingSystem() : shader("resources/shaders/vertex.txt", "resources/shaders/fragment.txt"),
									 lightShader("resources/shaders/lightSourceVertex.txt", "resources/shaders/lightSourceFragment.txt"),
									 borderShader("resources/shaders/lightSourceVertex.txt", "resources/shaders/borderFragment.txt"),
									 simpleShader("resources/shaders/simpleVertex.txt", "resources/shaders/simpleFragment.txt"),
									 depthShader("resources/shaders/depthVertex.txt", "resources/shaders/depthFragment.txt"),
									 debugShader("resources/shaders/debugVertex.txt", "resources/shaders/debugFragment.txt"),
									 skyboxShader("resources/shaders/skyboxVertex.txt", "resources/shaders/skyboxFragment.txt")
{
	// Initialize matrices
	this->projMatrix = glm::mat4(1.0f);
	this->viewMatrix = glm::mat4(1.0f);

	// const values for the whole level
	this->roughOrt.left = 280.0f;
	this->roughOrt.right = -284.0f;
	this->roughOrt.bottom = -206.0f;
	this->roughOrt.top = 241.0f;
	this->roughOrt.nearPlane = 0.1f;
	this->roughOrt.farPlane = 1400.0f;

	glm::mat4 lightProjMatrix = glm::ortho(this->roughOrt.left,
		this->roughOrt.right,
		this->roughOrt.bottom,
		this->roughOrt.top,
		this->roughOrt.nearPlane,
		this->roughOrt.farPlane);

	// Directional light position
	this->lightPos = glm::vec3(-0.1f, 1000.0f, -200.0f);
	this->lightDir = glm::normalize(glm::vec3(0) - this->lightPos);
	this->lightViewMatrix = glm::lookAt(this->lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	// Lo-res shadow map light space matrix
	this->loResLightSpaceMatrix = lightProjMatrix * this->lightViewMatrix;

	// Shadow map viewport sizes
	this->shadowHiRes = 4096;
	this->shadowLoRes = 2048;

	// Populate the models
	this->models.reserve(g_scene.entityCount());
	loadModels();

	glEnable(GL_DEPTH_TEST); // Turn on depth testing
	glDepthFunc(GL_LESS); // Should be default but make it explicit

	glEnable(GL_CULL_FACE); // Enable face culling
	glCullFace(GL_BACK); // Turn on back-face culling initially

	// Rendering uniforms
	shader.use();
	this->modelLoc = glGetUniformLocation(getShaderId(), "model");
	this->texLoc = glGetUniformLocation(getShaderId(), "textured");
	this->viewLoc = glGetUniformLocation(getShaderId(), "view");
	this->projLoc = glGetUniformLocation(getShaderId(), "proj");

	// Initialize shadow maps
	this->initShadows();

	// Initialize skybox
	this->initSkybox();

	this->initDebugQuad();

	NavMesh navMeshPoints;
	navMesh = navMeshPoints.getWireframe();

	// Multiplayer test
	glGenFramebuffers(1, &this->fourPlayerFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, this->fourPlayerFBO);

	glActiveTexture(GL_TEXTURE16);
	glGenTextures(1, &this->fourPlayerTex);
	glBindTexture(GL_TEXTURE_2D, this->fourPlayerTex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, g_systems.width, g_systems.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->fourPlayerTex, 0);

	glGenRenderbuffers(1, &this->fourPlayerRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, this->fourPlayerRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, g_systems.width, g_systems.height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->fourPlayerRBO);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer did not complete.\n";

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

RenderingSystem::~RenderingSystem() { }

void RenderingSystem::initShadows()
{
	// Rough shadows ------------------------------------------------------------------------------
	// Configure depth map FBO
	glGenFramebuffers(1, &this->roughDepthMapFBO);

	// Create depth texture
	glGenTextures(1, &this->roughDepthMapTex);
	glActiveTexture(GL_TEXTURE24);
	glBindTexture(GL_TEXTURE_2D, this->roughDepthMapTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, this->shadowLoRes, this->shadowLoRes, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	// Attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, this->roughDepthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->roughDepthMapTex, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// High resolution shadows ---------------------------------------------------------------------
	
	this->shadowWidth = this->shadowHiRes;// / g_scene.numPlayers;
	this->shadowHeight = this->shadowHiRes;// / g_scene.numPlayers;

	/*
	if (g_scene.numPlayers == 3)
	{
		this->shadowWidth = g_systems.width / 4;
		this->shadowHeight = g_systems.height / 4;
	}
	*/

	// Configure depth map FBO
	glGenFramebuffers(1, &this->p1ShadowsFBO);

	// Create depth texture
	glGenTextures(1, &this->p1ShadowsTex);
	glActiveTexture(GL_TEXTURE25);
	glBindTexture(GL_TEXTURE_2D, this->p1ShadowsTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor3[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor3);

	// Attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, this->p1ShadowsFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->p1ShadowsTex, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Configure depth map FBO
	glGenFramebuffers(1, &this->p2ShadowsFBO);

	// Create depth texture
	glGenTextures(1, &this->p2ShadowsTex);
	glActiveTexture(GL_TEXTURE25);
	glBindTexture(GL_TEXTURE_2D, this->p2ShadowsTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor3);

	// Attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, this->p2ShadowsFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->p2ShadowsTex, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Configure depth map FBO
	glGenFramebuffers(1, &this->p3ShadowsFBO);

	// Create depth texture
	glGenTextures(1, &this->p3ShadowsTex);
	glActiveTexture(GL_TEXTURE25);
	glBindTexture(GL_TEXTURE_2D, this->p3ShadowsTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor3);

	// Attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, this->p3ShadowsFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->p3ShadowsTex, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Configure depth map FBO
	glGenFramebuffers(1, &this->p4ShadowsFBO);

	// Create depth texture
	glGenTextures(1, &this->p4ShadowsTex);
	glActiveTexture(GL_TEXTURE25);
	glBindTexture(GL_TEXTURE_2D, this->p4ShadowsTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor3);

	// Attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, this->p4ShadowsFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->p4ShadowsTex, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderingSystem::initSkybox()
{
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	glGenVertexArrays(1, &this->skyboxVAO);
	glGenBuffers(1, &this->skyboxVBO);
	glBindVertexArray(this->skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glGenTextures(1, &this->cubeMap);
	glActiveTexture(GL_TEXTURE23);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->cubeMap);

	std::vector<std::string> faces;
	faces.push_back("resources/textures/skybox/x.jpg");
	faces.push_back("resources/textures/skybox/x.jpg");
	faces.push_back("resources/textures/skybox/z.jpg");
	faces.push_back("resources/textures/skybox/x.jpg");
	faces.push_back("resources/textures/skybox/z.jpg");
	faces.push_back("resources/textures/skybox/z.jpg");

	int width, height, nrChannels;
	unsigned char* data;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

unsigned int RenderingSystem::getShaderId()
{
	return this->shader.getId();
}

Shader& RenderingSystem::getShader()
{
	return this->shader;
}

void RenderingSystem::loadModels()
{
	unsigned int index = 0;

	//-----------------------------------------------------------------------------------
	// Player models
	//-----------------------------------------------------------------------------------
	std::string breadbusPath = "resources/models/breadbus/breadbus.obj";
	std::string pancakebusPath = "resources/models/breadbus/pancakebus.obj";
	std::string baguettebusPath = "resources/models/breadbus/baguettebus.obj";
	std::string cakebusPath = "resources/models/breadbus/cakebus.obj";

	// Player 1
	this->models.emplace_back(Model(&breadbusPath[0]));
	g_scene.getEntity("player1")->attachComponent(&(this->models[index++]), "model");

	// Player 2
	this->models.emplace_back(Model(&pancakebusPath[0]));
	g_scene.getEntity("player2")->attachComponent(&(this->models[index++]), "model");

	// Player 3
	this->models.emplace_back(Model(&cakebusPath[0]));
	g_scene.getEntity("player3")->attachComponent(&(this->models[index++]), "model");

	// Player 4
	this->models.emplace_back(Model(&baguettebusPath[0]));
	g_scene.getEntity("player4")->attachComponent(&(this->models[index++]), "model");

	//-----------------------------------------------------------------------------------
	// Environment models
	//-----------------------------------------------------------------------------------
	// Ground
	std::string groundPath = "resources/models/kitchen/kitchen.obj";
	this->models.emplace_back(Model(&groundPath[0]));
	g_scene.getEntity("countertop")->attachComponent(&(this->models[index++]), "model");

	// Fan
	std::string fanPath = "resources/models/kitchen/fan.obj";
	this->models.emplace_back(Model(&fanPath[0]));
	g_scene.getEntity("fan")->attachComponent(&(this->models[index++]), "model");

	//-----------------------------------------------------------------------------------
	// Ingredient models
	//-----------------------------------------------------------------------------------
	// Cheese
	std::string cheesePath = "resources/models/ingredients/cheese.obj";
	this->models.emplace_back(Model(&cheesePath[0]));
	g_scene.getEntity("cheese")->attachComponent(&(this->models[index++]), "model");

	// Sausage
	std::string sausagePath = "resources/models/ingredients/sausage.obj";
	this->models.emplace_back(Model(&sausagePath[0]));
	g_scene.getEntity("sausage")->attachComponent(&(this->models[index++]), "model");

	// Tomato
	std::string tomatoPath = "resources/models/ingredients/tomato.obj";
	this->models.emplace_back(Model(&tomatoPath[0]));
	g_scene.getEntity("tomato")->attachComponent(&(this->models[index++]), "model");

	// Dough
	std::string doughPath = "resources/models/ingredients/dough.obj";
	this->models.emplace_back(Model(&doughPath[0]));
	g_scene.getEntity("dough")->attachComponent(&(this->models[index++]), "model");

	// Carrot
	std::string carrotPath = "resources/models/ingredients/carrot.obj";
	this->models.emplace_back(Model(&carrotPath[0]));
	g_scene.getEntity("carrot")->attachComponent(&(this->models[index++]), "model");

	// Lettuce
	std::string lettucePath = "resources/models/ingredients/lettuce.obj";
	this->models.emplace_back(Model(&lettucePath[0]));
	g_scene.getEntity("lettuce")->attachComponent(&(this->models[index++]), "model");

	// Parsnip
	std::string parsnipPath = "resources/models/ingredients/parsnip.obj";
	this->models.emplace_back(Model(&parsnipPath[0]));
	g_scene.getEntity("parsnip")->attachComponent(&(this->models[index++]), "model");

	// Rice
	std::string ricePath = "resources/models/ingredients/rice.obj";
	this->models.emplace_back(Model(&ricePath[0]));
	g_scene.getEntity("rice")->attachComponent(&(this->models[index++]), "model");

	// Egg
	std::string eggPath = "resources/models/ingredients/egg.obj";
	this->models.emplace_back(Model(&eggPath[0]));
	g_scene.getEntity("egg")->attachComponent(&(this->models[index++]), "model");

	// Chicken
	std::string chickenPath = "resources/models/ingredients/chicken.obj";
	this->models.emplace_back(Model(&chickenPath[0]));
	g_scene.getEntity("chicken")->attachComponent(&(this->models[index++]), "model");

	// Peas
	std::string peasPath = "resources/models/ingredients/peas.obj";
	this->models.emplace_back(Model(&peasPath[0]));
	g_scene.getEntity("peas")->attachComponent(&(this->models[index++]), "model");

	//// Soupbase
	//std::string soupbasePath = "resources/models/ingredients/soupbase.obj";
	//this->models.emplace_back(Model(&soupbasePath[0]));
	//g_scene.getEntity("soupbase")->attachComponent(&(this->models[17]), "model");

	//// Pumpkin
	//std::string pumpkinPath = "resources/models/ingredients/pumpkin.obj";
	//this->models.emplace_back(Model(&pumpkinPath[0]));
	//g_scene.getEntity("pumpkin")->attachComponent(&(this->models[18]), "model");

	//-----------------------------------------------------------------------------------
	// Debug models
	//-----------------------------------------------------------------------------------
	std::string testPath = "resources/models/kitchen/lego.obj";
	this->models.emplace_back(Model(&testPath[0]));
	g_scene.getEntity("test")->attachComponent(&(this->models[index++]), "model");
}

void RenderingSystem::initDebugQuad()
{
	// Create quad VAO for final default framebuffer image render
	float quadVerts[] = {
		// Position   // Tex Coords
		-1.0f,  1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, 1.0f, 0.0f,

		-1.0f,  1.0f, 0.0f, 1.0f,
		 1.0f, -1.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 1.0f,
	};

	glGenVertexArrays(1, &this->quadVAO);
	glGenBuffers(1, &this->quadVBO);
	glBindVertexArray(this->quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), &quadVerts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

void RenderingSystem::init4PlayerQuad()
{
	float fourPlayerVerts[] = {
		// Position   // Tex Coords
		// First quad
		-1.0f,  1.0f, 0.0f, 1.0f,
		-1.0f,  0.0f, 0.0f, 0.0f,
		 0.0f,  0.0f, 1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f, 1.0f,
		 0.0f,  0.0f, 1.0f, 0.0f,
		 0.0f,  1.0f, 1.0f, 1.0f,

		// Second quad
		 0.0f,  1.0f, 0.0f, 1.0f,
		 0.0f,  0.0f, 0.0f, 0.0f, 
		 1.0f,  0.0f, 1.0f, 0.0f,
		 0.0f,  1.0f, 0.0f, 1.0f, 
		 1.0f,  0.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 1.0f,

		// Third quad
		-1.0f,  0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f,
		 0.0f, -1.0f, 1.0f, 0.0f,
		-1.0f,  0.0f, 0.0f, 1.0f,
		 0.0f, -1.0f, 1.0f, 0.0f,
		 0.0f,  0.0f, 1.0f, 1.0f,

		// Forth quad
		 0.0f,  0.0f, 0.0f, 1.0f,
		 0.0f, -1.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, 1.0f, 0.0f,
		 0.0f,  0.0f, 0.0f, 1.0f,
		 1.0f, -1.0f, 1.0f, 0.0f,
		 1.0f,  0.0f, 1.0f, 1.0f,
	};

	glGenVertexArrays(1, &this->fourPlayerVAO);
	glGenBuffers(1, &this->fourPlayerVBO);
	glBindVertexArray(fourPlayerVAO);
	glBindBuffer(GL_ARRAY_BUFFER, fourPlayerVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fourPlayerVerts), &fourPlayerVerts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

void RenderingSystem::updateCamera(Camera* camera, int playerNum)
{
	// Update the camera for the new frame
	camera->update(playerNum);

	// Get new view matrix from Camera and update the regular Shader
	glUniformMatrix4fv(this->viewLoc, 1, GL_FALSE, glm::value_ptr(camera->viewMatrix));

	// Update regular Shader with the Camera's new position
	unsigned int viewPosLoc = glGetUniformLocation(this->shader.getId(), "viewPos");
	glUniform3f(viewPosLoc, camera->position.x, camera->position.y, camera->position.z);

	// Update Shader with Camera's new projection matrix
	glUniformMatrix4fv(this->projLoc, 1, GL_FALSE, glm::value_ptr(camera->projMatrix));
}

void RenderingSystem::createLoResShadowMap()
{
	// Switch to depth map shader and set thew new light space matrix uniform
	depthShader.use();
	depthShader.setMat4("lightSpaceMatrix", this->loResLightSpaceMatrix);

	// Change viewport for shadow map settings
	glViewport(0, 0, this->shadowLoRes, this->shadowLoRes);

	// Draw to FBO (texture)
	glBindFramebuffer(GL_FRAMEBUFFER, this->roughDepthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT); // Switch to front face culling to reduce peter panning
	renderShadowMap();
	glCullFace(GL_BACK);
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind shadow map FBO
}

void RenderingSystem::createHiResShadowMap(const std::string name)
{
	const float buffer = 70.0f;

	glm::vec3 pos = g_scene.getEntity(name)->getTransform()->position;

	glm::vec4 left = glm::vec4(pos.x + buffer, pos.y, pos.z, 1.0f);
	glm::vec4 right = glm::vec4(pos.x - buffer, pos.y, pos.z, 1.0f);
	glm::vec4 top = glm::vec4(pos.x, pos.y, pos.z + buffer, 1.0f);
	glm::vec4 bottom = glm::vec4(pos.x, pos.y, pos.z + buffer, 1.0f);

	left = this->lightViewMatrix * left;
	right = this->lightViewMatrix * right;
	top = this->lightViewMatrix * top;
	bottom = this->lightViewMatrix * bottom;

	Orthogonal ort;
	ort.left = -(pos.x + buffer);
	ort.right = -(pos.x - buffer);
	ort.top = pos.z + buffer;
	ort.bottom = pos.z - buffer;
	ort.nearPlane = 0.1f;
	ort.farPlane = 1400.0f;

	glm::mat4 lightProjMatrix, lightSpaceMatrix;

	lightProjMatrix = glm::ortho(ort.left, ort.right, ort.bottom, ort.top, ort.nearPlane, ort.farPlane);
	lightSpaceMatrix = lightProjMatrix * this->lightViewMatrix;

	if (name == "player1")
	{
		this->p1LightSpaceMatrix = lightSpaceMatrix;
		glBindFramebuffer(GL_FRAMEBUFFER, this->p1ShadowsFBO);
	}
	else if (name == "player2")
	{
		this->p2LightSpaceMatrix = lightSpaceMatrix;
		glBindFramebuffer(GL_FRAMEBUFFER, this->p2ShadowsFBO);
	}	
	else if (name == "player3")
	{
		this->p3LightSpaceMatrix = lightSpaceMatrix;
		glBindFramebuffer(GL_FRAMEBUFFER, this->p3ShadowsFBO);
	}	
	else if (name == "player4")
	{
		this->p4LightSpaceMatrix = lightSpaceMatrix;
		glBindFramebuffer(GL_FRAMEBUFFER, this->p4ShadowsFBO);
	}

	// Switch to depth map shader and update it's light space matrix uniform
	depthShader.use();
	depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

	// Resize the viewport for hi-res shadows
	glViewport(0, 0, this->shadowWidth, this->shadowHeight);

	// Draw to a FBO (texture)
	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);
	renderShadowMap();
	glCullFace(GL_BACK);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderingSystem::renderShadowMap()
{
	// Switch to the depth map shader
	depthShader.use();

	// Get model unfirom location in shader
	unsigned int modelLoc = glGetUniformLocation(this->depthShader.getId(), "model");

	// Render scene to depthMapFBO
	for (auto it = this->models.begin(); it < this->models.end(); it++)
	{
		// Update model matrix
		Transform* ownerTransform = it->owner->getTransform();
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(ownerTransform->getModelMatrix()));

		it->drawDepthMap(this->depthShader);
	}
}

void RenderingSystem::renderScene(const std::string name)
{
	// Reset viewport
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Switch to the regular shader
	this->shader.use();

	glm::mat4 lightSpaceMatrix, modelMatrix;
	glm::vec3 pos;
	Camera* camera = nullptr;

	glActiveTexture(GL_TEXTURE25);

	if (name == "player1")
	{
		glBindTexture(GL_TEXTURE_2D, this->p1ShadowsTex);
		lightSpaceMatrix = this->p1LightSpaceMatrix;

		if (g_scene.numPlayers == 1)
			glViewport(0, 0, g_systems.width, g_systems.height);
		else if (g_scene.numPlayers == 2)
			glViewport(g_systems.width / 8, g_systems.height / 2, g_systems.width * (6.0f / 8.0f), g_systems.height / 2);
		else
			glViewport(0, g_systems.height / 2, g_systems.width / 2, g_systems.height / 2);

		camera = g_scene.p1Camera;
		updateCamera(camera, 1);

		// Check if there is a wall between the player and the camera and get the position if there is
		if (g_systems.physics->p1CameraHit)
		{
			camera->recalculateViewMatrix(g_systems.physics->p1CameraHitPos);
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera->viewMatrix));
		}
	}
	else if (name == "player2")
	{
		glBindTexture(GL_TEXTURE_2D, this->p2ShadowsTex);
		lightSpaceMatrix = this->p2LightSpaceMatrix;

		if (g_scene.numPlayers == 2)
			glViewport(g_systems.width / 8, 0, g_systems.width * (6.0f / 8.0f), g_systems.height / 2);
		else
			glViewport(g_systems.width / 2, g_systems.height / 2, g_systems.width / 2, g_systems.height / 2);

		camera = g_scene.p2Camera;
		updateCamera(camera, 2);

		// Check if there is a wall between the player and the camera and get the position if there is
		if (g_systems.physics->p2CameraHit)
		{
			camera->recalculateViewMatrix(g_systems.physics->p2CameraHitPos);
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera->viewMatrix));
		}
	}	
	else if (name == "player3")
	{
		glBindTexture(GL_TEXTURE_2D, this->p3ShadowsTex);
		lightSpaceMatrix = this->p3LightSpaceMatrix;
		glViewport(0, 0, g_systems.width / 2, g_systems.height / 2);

		if (g_scene.numPlayers == 3)
			glViewport(g_systems.width / 4, 0, g_systems.width / 2, g_systems.height / 2);
		else
			glViewport(0, 0, g_systems.width / 2, g_systems.height / 2);

		camera = g_scene.p3Camera;
		updateCamera(camera, 3);

		// Check if there is a wall between the player and the camera and get the position if there is
		if (g_systems.physics->p3CameraHit)
		{
			camera->recalculateViewMatrix(g_systems.physics->p3CameraHitPos);
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera->viewMatrix));
		}
	}	
	else if (name == "player4")
	{
		glBindTexture(GL_TEXTURE_2D, this->p4ShadowsTex);
		lightSpaceMatrix = this->p4LightSpaceMatrix;
		glViewport(g_systems.width / 2, 0, g_systems.width / 2, g_systems.height / 2);

		camera = g_scene.p4Camera;
		updateCamera(camera, 4);

		// Check if there is a wall between the player and the camera and get the position if there is
		if (g_systems.physics->p4CameraHit)
		{
			camera->recalculateViewMatrix(g_systems.physics->p4CameraHitPos);
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera->viewMatrix));
		}
	}
	else
		return; // Uh-oh, something has gone wrong

	// Update uniforms
	this->shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
	this->shader.setMat4("roughLightSpaceMatrix", this->loResLightSpaceMatrix);
	this->shader.setVec3("playerPos", g_scene.getEntity(name)->getTransform()->position);
	this->shader.setMat4("playerModelMatrix", g_scene.getEntity(name)->getTransform()->getModelMatrix());

	// Bind rough shadow map texture
	glActiveTexture(GL_TEXTURE24);
	glBindTexture(GL_TEXTURE_2D, this->roughDepthMapTex);
	glUniform1i(glGetUniformLocation(this->shader.getId(), "roughShadowMap"), 24);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE, 0);

	// Bind high res shadow map texture
	glUniform1i(glGetUniformLocation(this->shader.getId(), "shadowMap"), 25);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE, 0);

	if (DEBUG_NAVMESH)
	{
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Transform().getModelMatrix()));
		glUniform1i(texLoc, 0); // Turn textures off
		navMesh.setWireframe(true);
		navMesh.draw(getShader());
	}

	// Iterate through all the models in the scene and render them at their new transforms
	for (int i = 0; i < models.size(); i++)
	{
		Transform* ownerTransform = models[i].owner->getTransform();

		// Spin the fan
		if (models[i].owner->name == "fan")
		{
			models[i].owner->getTransform()->rotation = glm::vec3(0.0f, glfwGetTime() * 50.0f, 0.0f);
			models[i].owner->getTransform()->update();
		}

		// Update model matrix
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(ownerTransform->getModelMatrix()));

		if (i < 3) // Use material info for first 3 player models
		{
			glUniform1i(texLoc, 0);
			models[i].draw(this->shader);
		}
		else // Use texture images for everything else
		{
			if (i == 17)
			{
				//models[i].owner->getTransform()->position = glm::vec3(88.17f, -13.15f, 160.71f);
				//models[i].owner->getTransform()->rotation = glm::vec3(0.0f, 200.50f, 0.0f);
				models[i].owner->getTransform()->position = glm::vec3(0.0f, 10.0f, 0.0f);
				models[i].owner->getTransform()->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
				models[i].owner->getTransform()->update();
			}

			glUniform1i(texLoc, 1);
			models[i].draw(this->shader);
		}
	}
	
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // Reset to default FBO

	drawSkybox(camera);
}

void RenderingSystem::update()
{
	// Animate ingredients
	if (g_systems.loop->gameStage == GameLoopMode::MAIN_GAME_PLAY && !g_systems.loop->isPaused)
	{
		for (unsigned int i = 6; i <= 16; i++)
		{
			models[i].owner->getTransform()->translate(glm::vec3(0.0f, glm::abs(glm::cos(glfwGetTime())) * 2.0f, 0.0f));
		}
	}

	// Step 1. Create the lo-res shadow map for the whole level
	createLoResShadowMap();

	// Step 2. Create the hi-res shadow maps for the players
	for (unsigned int i = 1; i <= g_scene.numPlayers; i++)
	{
		std::string playerNum = "player" + std::to_string(i);
		createHiResShadowMap(playerNum);
	}

	if (this->shadowDebugMode < 1)
	{
		// Step 3. Render the scene from each player's perspective
		for (unsigned int i = 1; i <= g_scene.numPlayers; i++)
		{
			std::string playerNum = "player" + std::to_string(i);
			renderScene(playerNum);

			if (!g_systems.loop->isPaused)
				g_systems.ui->updatePlayer(i, g_systems.loop->gameStage);
		}
	}
	else if (this->shadowDebugMode >= 1)
	{
		this->renderDebugShadowMap();
	}
	
	if (g_systems.loop->isPaused)
	{
		glViewport(0, 0, g_systems.width, g_systems.height);
		g_systems.ui->showPauseMenu(g_systems.loop->pauseMenuSelection);
	}
	else if (g_systems.loop->gameStage == GameLoopMode::END_GAME)
	{
		glViewport(0, 0, g_systems.width, g_systems.height);
		g_systems.ui->updateEndGame(g_systems.loop->endScreenGenerated);
	}
	else if (g_systems.loop->gameStage == GameLoopMode::START_COUNTDOWN) {
		glViewport(0, 0, g_systems.width, g_systems.height);
		g_systems.ui->updateCountdown(g_systems.loop->countdownStage);
	}
}

void RenderingSystem::renderOnePlayerQuad()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	glDisable(GL_DEPTH_TEST);

	glClearColor(0.1f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	this->simpleShader.use();
	this->simpleShader.setInt("screenTexture", 16);

	glBindVertexArray(this->quadVAO);
	glActiveTexture(GL_TEXTURE16);
	glBindTexture(GL_TEXTURE_2D, this->fourPlayerTex);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
}

void RenderingSystem::renderFourPlayerQuad()
{
	this->simpleShader.use();

	// Bind VAO
	glBindVertexArray(this->fourPlayerVAO);

	// Turn depth testing off for 2D rendering
	glDisable(GL_DEPTH_TEST);

	// Draw to default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, g_systems.width, g_systems.height);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT); //; | GL_DEPTH_BUFFER_BIT);

	// Bind rendered texture
	glActiveTexture(GL_TEXTURE16);
	glBindTexture(GL_TEXTURE_2D, this->fourPlayerTex);

	// Render scene to viewport by applying textures to 2D quads
	this->simpleShader.setInt("screenTexture", 16);
	glDrawArrays(GL_TRIANGLES, 0, 24);
	glBindVertexArray(0);

	// Unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);

	// Re-enable depth testing for next frame
	glEnable(GL_DEPTH_TEST);
}

void RenderingSystem::renderDebugShadowMap()
{
	// Debug code for rendering the depthMap to viewport
	glViewport(0, 0, g_systems.width, g_systems.height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	this->depthShader.use();
	glActiveTexture(GL_TEXTURE25);

	if (this->shadowDebugMode == 1)
		glBindTexture(GL_TEXTURE_2D, this->roughDepthMapTex);
	if (this->shadowDebugMode == 2)
		glBindTexture(GL_TEXTURE_2D, this->p1ShadowsTex);

	glUniform1i(glGetUniformLocation(this->debugShader.getId(), "shadowMap"), 25);
	renderTexturedQuad();
	glBindTexture(GL_TEXTURE_2D, 0);
}

Model* RenderingSystem::getKitchenModel()
{
	return g_scene.getEntity("countertop")->getModel();
}

void RenderingSystem::renderTexturedQuad()
{
	// Draw to default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, g_systems.width, g_systems.height);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render scene to viewport by applying depthMap as texture to 2D quad
	this->debugShader.use();
	this->debugShader.setInt("depthMap", 25);
	glBindVertexArray(this->quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void RenderingSystem::drawSkybox(Camera* camera)
{
	glDepthFunc(GL_LEQUAL);
	this->skyboxShader.use();
	this->skyboxShader.setMat4("view", glm::mat4(glm::mat3(camera->viewMatrix)));
	this->skyboxShader.setMat4("projection", camera->projMatrix);
	this->skyboxShader.setInt("skybox", 23);

	glBindVertexArray(this->skyboxVAO);
	glActiveTexture(GL_TEXTURE23);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->cubeMap);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
}