#include <iostream>

#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

#include "../SystemManager.h"
#include "RenderingSystem.h"
#include "../Scene/Scene.h"
#include "../Scene/Entity.h"
#include <stbi/stb_image.h>

extern Scene g_scene;
extern SystemManager g_systems;

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

	// Orthographic projection for shadow map settings
	this->ort.left = 281.0f;
	this->ort.right = -279.0f;
	this->ort.bottom = -205.0f;
	this->ort.top = 197.0f;
	this->ort.nearPlane = 0.1f;
	this->ort.farPlane = 2000.0f;

	// Directional light position
	this->lightPos = glm::vec3(-0.1f, 1000.0f, -200.0f);
	this->lightDir = glm::normalize(glm::vec3(0) - this->lightPos);

	// Shadow map viewport size
	this->shadowHiRes = 4096;
	this->shadowLoRes = 2048;

	this->maxBias = 0.001f;
	this->minBias = 0.00005f;
	this->maxRoughBias = 0.005f;
	this->minRoughBias = 0.0005f;

	this->models.reserve(g_scene.entityCount() * 2); // Create space for models
	loadModels(); // Load model files into the models vector

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

	//glEnable(GL_STENCIL_TEST); // Turn on stencil testing
	//glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	// Camera
	Transform transform = Transform();
	transform.position = glm::vec3(1.0f);
	setupCameras(&transform); // Setup the camera
	
	// --------------------------------------------------------------------------------------------
	// SHADOWS
	// --------------------------------------------------------------------------------------------
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
	// Configure depth map FBO
	glGenFramebuffers(1, &this->depthMapFBO);
	
	// Create depth texture
	glGenTextures(1, &this->depthMapTex);
	glActiveTexture(GL_TEXTURE25);
	glBindTexture(GL_TEXTURE_2D, this->depthMapTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, this->shadowHiRes, this->shadowHiRes, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor2[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor2);

	// Attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, this->depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->depthMapTex, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// --------------------------------------------------------------------------------------------
	// QUAD FOR RENDERING FINAL PASS
	// --------------------------------------------------------------------------------------------
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
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), &quadVerts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	
	// --------------------------------------------------------------------------------------------
	// SKYBOX
	// --------------------------------------------------------------------------------------------
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
	faces.push_back("resources/textures/skybox/right.jpg");
	faces.push_back("resources/textures/skybox/left.jpg");
	faces.push_back("resources/textures/skybox/top.jpg");
	faces.push_back("resources/textures/skybox/bottom.jpg");
	faces.push_back("resources/textures/skybox/front.jpg");
	faces.push_back("resources/textures/skybox/back.jpg");

	int width, height, nrChannels;
	unsigned char* data;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		else
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
		
		stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

RenderingSystem::~RenderingSystem() { }

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
	//-----------------------------------------------------------------------------------
	// Player models
	//-----------------------------------------------------------------------------------
	std::string breadmobilePath = "resources/models/breadbus/breadbus.obj";
	
	// Player 1
	this->models.emplace_back(Model(&breadmobilePath[0]));
	g_scene.getEntity("player1")->attachComponent(&(this->models[0]), "model");
	
	// Player 2
	this->models.emplace_back(Model(&breadmobilePath[0]));
	g_scene.getEntity("player2")->attachComponent(&(this->models[1]), "model");
	
	// Player 3
	this->models.emplace_back(Model(&breadmobilePath[0]));
	g_scene.getEntity("player3")->attachComponent(&(this->models[2]), "model");
	
	// Player 4
	this->models.emplace_back(Model(&breadmobilePath[0]));
	g_scene.getEntity("player4")->attachComponent(&(this->models[3]), "model");

	//-----------------------------------------------------------------------------------
	// Environment models
	//-----------------------------------------------------------------------------------
	// Ground
	std::string groundPath = "resources/models/kitchen/kitchen.obj";
	this->models.emplace_back(Model(&groundPath[0])); 
	g_scene.getEntity("countertop")->attachComponent(&(this->models[4]), "model");

	//-----------------------------------------------------------------------------------
	// Ingredient models
	//-----------------------------------------------------------------------------------
	// Cheese
	std::string cheesePath = "resources/models/ingredients/cheese.obj";
	this->models.emplace_back(Model(&cheesePath[0]));
	g_scene.getEntity("cheese")->attachComponent(&(this->models[5]), "model");

	// Sausage
	std::string sausagePath = "resources/models/ingredients/sausage.obj";
	this->models.emplace_back(Model(&sausagePath[0]));
	g_scene.getEntity("sausage")->attachComponent(&(this->models[6]), "model");

	// Tomato
	std::string tomatoPath = "resources/models/ingredients/tomato.obj";
	this->models.emplace_back(Model(&tomatoPath[0]));
	g_scene.getEntity("tomato")->attachComponent(&(this->models[7]), "model");

	// Dough
	std::string doughPath = "resources/models/ingredients/dough.obj";
	this->models.emplace_back(Model(&doughPath[0]));
	g_scene.getEntity("dough")->attachComponent(&(this->models[8]), "model");

	// Carrot
	std::string carrotPath = "resources/models/ingredients/carrot.obj";
	this->models.emplace_back(Model(&carrotPath[0]));
	g_scene.getEntity("carrot")->attachComponent(&(this->models[9]), "model");

	// Lettuce
	std::string lettucePath = "resources/models/ingredients/lettuce.obj";
	this->models.emplace_back(Model(&lettucePath[0]));
	g_scene.getEntity("lettuce")->attachComponent(&(this->models[10]), "model");

	// Parsnip
	std::string parsnipPath = "resources/models/ingredients/parsnip.obj";
	this->models.emplace_back(Model(&parsnipPath[0]));
	g_scene.getEntity("parsnip")->attachComponent(&(this->models[11]), "model");

	// Rice
	std::string ricePath = "resources/models/ingredients/rice.obj";
	this->models.emplace_back(Model(&ricePath[0]));
	g_scene.getEntity("rice")->attachComponent(&(this->models[12]), "model");

	// Egg
	std::string eggPath = "resources/models/ingredients/egg.obj";
	this->models.emplace_back(Model(&eggPath[0]));
	g_scene.getEntity("egg")->attachComponent(&(this->models[13]), "model");

	// Chicken
	std::string chickenPath = "resources/models/ingredients/chicken.obj";
	this->models.emplace_back(Model(&chickenPath[0]));
	g_scene.getEntity("chicken")->attachComponent(&(this->models[14]), "model");

	// Peas
	std::string peasPath = "resources/models/ingredients/peas.obj";
	this->models.emplace_back(Model(&peasPath[0]));
	g_scene.getEntity("peas")->attachComponent(&(this->models[15]), "model");

	// Soupbase
	std::string soupbasePath = "resources/models/ingredients/soupbase.obj";
	this->models.emplace_back(Model(&soupbasePath[0]));
	g_scene.getEntity("soupbase")->attachComponent(&(this->models[16]), "model");

	// Pumpkin
	std::string pumpkinPath = "resources/models/ingredients/pumpkin.obj";
	this->models.emplace_back(Model(&pumpkinPath[0]));
	g_scene.getEntity("pumpkin")->attachComponent(&(this->models[17]), "model");

	//-----------------------------------------------------------------------------------
	// Debug models
	//-----------------------------------------------------------------------------------
	std::string testPath = "resources/models/ball/ball.obj";
	this->models.emplace_back(Model(&testPath[0]));
	g_scene.getEntity("test")->attachComponent(&(this->models[18]), "model");
}

void RenderingSystem::setupCameras(Transform* player1Transform)
{
	// Get view matrix from Camera and update the Shader
	this->viewMatrix = g_scene.camera.getViewMatrix(player1Transform);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(this->viewMatrix));

	// Tell shader about camera position
	unsigned int viewPosLoc = glGetUniformLocation(getShaderId(), "viewPos");
	glm::vec3 cameraPos = g_scene.camera.position;
	glUniform3f(viewPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);

	// Projection matrix will be handled by the Camera class in the future
	this->projMatrix = glm::mat4(1.0f);
	float screenWidth = g_systems.width;
	float screenHeight = g_systems.height;

	this->projMatrix = glm::perspective(glm::radians(g_scene.camera.getPerspective()), screenWidth / screenHeight, 0.1f, 1000.0f);
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(this->projMatrix));
}

// Orthographic projection for the depth map that shows the entire level layout for rough shadows at a distance
void RenderingSystem::updateRoughOrtho()
{
	this->ort.left = 280.0f;
	this->ort.right = -284.0f;
	this->ort.bottom = -206.0f;
	this->ort.top = 241.0f;
	this->ort.nearPlane = 0.1f;
	this->ort.farPlane = 1400.0f;
}

// Moves the orthographic projection used for the depth map so that it follows the player for high res shadows
void RenderingSystem::updateOrtho(glm::mat4 lightView)
{
	glm::vec3 p1Pos = g_scene.getEntity("player1")->getTransform()->position;
	glm::mat4 p1ModelMatrix = g_scene.getEntity("player1")->getTransform()->getModelMatrix();

	glm::vec4 left = glm::vec4(p1Pos.x + 50.0f, p1Pos.y, p1Pos.z, 1.0f);
	glm::vec4 right = glm::vec4(p1Pos.x - 50.0f, p1Pos.y, p1Pos.z, 1.0f);
	glm::vec4 top = glm::vec4(p1Pos.x, p1Pos.y, p1Pos.z + 50.0f, 1.0f);
	glm::vec4 bottom = glm::vec4(p1Pos.x, p1Pos.y, p1Pos.z + 50.0f, 1.0f);

	left = lightView * left;
	right = lightView * right;
	top = lightView * top;
	bottom = lightView * bottom;

	this->ort.left = -(p1Pos.x + 50.0f);
	this->ort.right = -(p1Pos.x - 50.0f);
	this->ort.top = p1Pos.z + 50.0f;
	this->ort.bottom = p1Pos.z - 50.0f;;
	this->ort.nearPlane = 0.1f;
	this->ort.farPlane = 1400.0f;
}

void RenderingSystem::update()
{
	// Rough shadows -----------------------------------------------------------------
	// Render roughDepthMap to texture (from light's perspective)
	glm::mat4 lightProjection, lightView, roughLightSpaceMatrix, lightSpaceMatrix;
	updateRoughOrtho();
	lightProjection = glm::ortho(ort.left, ort.right, ort.bottom, ort.top, ort.nearPlane, ort.farPlane);
	lightView = glm::lookAt(this->lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	roughLightSpaceMatrix = lightProjection * lightView;

	depthShader.use();
	depthShader.setMat4("lightSpaceMatrix", roughLightSpaceMatrix);

	glViewport(0, 0, this->shadowLoRes, this->shadowLoRes);
	glBindFramebuffer(GL_FRAMEBUFFER, this->roughDepthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);
	renderShadowMap();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// High res shadows --------------------------------------------------------------
	// Render depthMap to texture (from light's perspective)
	lightView = glm::lookAt(this->lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	updateOrtho(lightView);
	lightProjection = glm::ortho(ort.left, ort.right, ort.bottom, ort.top, ort.nearPlane, ort.farPlane);
	lightSpaceMatrix = lightProjection * lightView;

	depthShader.use();
	depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

	glViewport(0, 0, this->shadowHiRes, this->shadowHiRes);
	glBindFramebuffer(GL_FRAMEBUFFER, this->depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	renderShadowMap();
	glCullFace(GL_BACK);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Regular render pass -------------------------------------------------------------
	// Debug code for rendering the depthMap to viewport
	if (g_systems.renderDebug)
		renderDebugShadowMap();
	else
	{
		// Render scene as normal using the generated depth/shadow map
		this->shader.use();
		this->shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		this->shader.setMat4("roughLightSpaceMatrix", roughLightSpaceMatrix);
		this->shader.setVec3("playerPos", g_scene.getEntity("player1")->getTransform()->position);
		this->shader.setMat4("playerModelMatrix", g_scene.getEntity("player1")->getTransform()->getModelMatrix());
		renderScene();
	}
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
		glBindTexture(GL_TEXTURE_2D, this->depthMapTex);
	//glUniform1i(glGetUniformLocation(this->debugShader.getId(), "shadowMap"), 25);
	renderTexturedQuad();
	glBindTexture(GL_TEXTURE_2D, 0);
}

Model* RenderingSystem::getKitchenModel()
{
	return g_scene.getEntity("countertop")->getModel();
}

void RenderingSystem::renderScene()
{
	// Reset viewport
	glViewport(0, 0, g_systems.width, g_systems.height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Update camera (MVP matrices)
	Transform* p1Transform = g_scene.getEntity("player1")->getTransform();
	g_scene.camera.updateCameraVectors(p1Transform);
	setupCameras(p1Transform);

	// Bind rough shadow map
	glActiveTexture(GL_TEXTURE24);
	glBindTexture(GL_TEXTURE_2D, this->roughDepthMapTex);
	glUniform1i(glGetUniformLocation(this->shader.getId(), "roughShadowMap"), 24);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE, 0);

	// Bind high res shadow map
	glActiveTexture(GL_TEXTURE25);
	glBindTexture(GL_TEXTURE_2D, this->depthMapTex);
	glUniform1i(glGetUniformLocation(this->shader.getId(), "shadowMap"), 25);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE, 0);

	glUniform1i(texLoc, 1);

	// Set shadow bias uniforms
	shader.setFloat("maxBias", this->maxBias);
	shader.setFloat("minBias", this->minBias);
	shader.setFloat("maxRoughBias", this->maxRoughBias);
	shader.setFloat("minRoughBias", this->minRoughBias);

	// Iterate through all the models in the scene and render them at their new transforms
	for (int i = 0; i < models.size(); i++)
	{
		Transform* ownerTransform = models[i].owner->getTransform();
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(ownerTransform->getModelMatrix()));

		if (i < 4) // Use material info for player models
		{
			glUniform1i(texLoc, 0);
			models[i].draw(this->shader);
		}
		else if (i >= 4) // Use texture images for everything else
		{
			//ownerTransform->update();

			glUniform1i(texLoc, 1);
			models[i].draw(this->shader);
		}
	}
}

void RenderingSystem::renderShadowMap()
{	
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

glm::mat4 RenderingSystem::calculateOrthoProjection()
{
	// Get the inverse of the view transform
	glm::mat4 inverseMatrix = glm::inverse(this->projMatrix * this->viewMatrix);

	// Get the frustum corners in world space coordinates
	std::vector<glm::vec4> corners;
	for (unsigned int x = 0; x < 2; x++)
	{
		for (unsigned int y = 0; y < 2; y++)
		{
			for (unsigned int z = 0; z < 2; z++)
			{
				glm::vec4 point = inverseMatrix * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
				corners.push_back(point / point.w); // Why?
			}
		}
	}

	// Find the center point of the regular camer'a frustum
	glm::vec3 center = glm::vec3(0);
	for (auto& v : corners)
	{
		v /= 5.0;
		center += glm::vec3(v);
	}

	center /= corners.size();

	// Calculate the light's view matrix
	glm::mat4 lightView = glm::lookAt(center + this->lightDir, center, glm::vec3(0.0f, 1.0f, 0.0f));

	// Transform frustum corner points to light space
	float minX = std::numeric_limits<float>::max();
	float maxX = std::numeric_limits<float>::min();
	float minY = std::numeric_limits<float>::max();
	float maxY = std::numeric_limits<float>::min();
	float minZ = std::numeric_limits<float>::max();
	float maxZ = std::numeric_limits<float>::min();
	
	for (auto& v : corners)
	{
		glm::vec4 transformed = lightView * v;
		minX = std::min(minX, transformed.x);
		maxX = std::min(maxX, transformed.x);
		minY = std::min(minY, transformed.y);
		maxY = std::min(maxY, transformed.y);
		minZ = std::min(minZ, transformed.z);
		maxZ = std::min(maxZ, transformed.z);
	}

	// Stretch near and far planes
	float zMult = 10.0f;
	if (minZ < 0)
		minZ *= zMult;
	else
		minZ /= zMult;
	if (maxZ < 0)
		maxZ /= zMult;
	else
		maxZ *= zMult;

	this->ort.left = maxX;
	this->ort.right = minX;
	this->ort.top = maxY;
	this->ort.bottom = minY;
	this->ort.nearPlane = minZ;
	this->ort.farPlane = maxZ;

	return glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
}

void RenderingSystem::drawSkybox()
{
	glDepthFunc(GL_LEQUAL);
	this->skyboxShader.use();
	this->skyboxShader.setMat4("view", this->viewMatrix);
	this->skyboxShader.setMat4("projection", this->projMatrix);
	this->skyboxShader.setInt("skybox", 23);

	glBindVertexArray(this->skyboxVAO);
	glActiveTexture(GL_TEXTURE23);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->cubeMap);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glDepthFunc(GL_LESS);
}