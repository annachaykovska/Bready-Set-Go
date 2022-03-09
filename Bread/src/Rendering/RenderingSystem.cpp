#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

#include "../SystemManager.h"
#include "RenderingSystem.h"
#include "../Scene/Scene.h"
#include "../Scene/Entity.h"

extern Scene g_scene;
extern SystemManager g_systems;

RenderingSystem::RenderingSystem() : shader("resources/shaders/vertex.txt", "resources/shaders/fragment.txt"),
									 lightShader("resources/shaders/lightSourceVertex.txt", "resources/shaders/lightSourceFragment.txt"),
									 borderShader("resources/shaders/lightSourceVertex.txt", "resources/shaders/borderFragment.txt"),
								     simpleShader("resources/shaders/simpleVertex.txt", "resources/shaders/simpleFragment.txt"),
									 depthShader("resources/shaders/depthVertex.txt", "resources/shaders/depthFragment.txt"),
									 debugShader("resources/shaders/debugVertex.txt", "resources/shaders/debugFragment.txt")
{
	this->screenWidth = 0;
	this->screenHeight = 0;

	this->ort.x = -100.0f;
	this->ort.y = 100.0f;
	this->ort.z = -100.0f;
	this->ort.w = 100.0f;
	this->ort.nearPlane = 0.1f;
	this->ort.farPlane = 600.0f;

	this->models.reserve(g_scene.count()); // Create space for models
	loadModels(); // Load model files into the models vector

	glEnable(GL_DEPTH_TEST);

	/*
	// Rendering uniforms
	this->modelLoc = glGetUniformLocation(getShaderId(), "model");
	this->texLoc = glGetUniformLocation(getShaderId(), "textured");
	this->viewLoc = glGetUniformLocation(getShaderId(), "view");
	this->projLoc = glGetUniformLocation(getShaderId(), "proj");

	glEnable(GL_DEPTH_TEST); // Turn on depth testing
	glDepthFunc(GL_LESS);

	//glEnable(GL_STENCIL_TEST); // Turn on stencil testing
	//glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	// Camera
	Transform transform = Transform();
	transform.position = glm::vec3(1.0f);
	setupCameras(&transform); // Setup the camera
	*/
	
	// SHADOWS

	// Configure depth map FBO
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	glGenFramebuffers(1, &this->depthMapFBO);
	
	// Create depth texture
	glGenTextures(1, &this->depthMapTex);
	glBindTexture(GL_TEXTURE_2D, this->depthMapTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, this->depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->depthMapTex, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Shader configuration
	debugShader.use();
	debugShader.setInt("depthMap", 0);

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
	std::string breadmobilePath = "resources/models/breadbus/breadbus2.obj";
	
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
	this->models.emplace_back(Model(&cheesePath[0])); // Cheese ingredient
	g_scene.getEntity("cheese")->attachComponent(&(this->models[5]), "model");

	// Sausage
	std::string sausagePath = "resources/models/ingredients/sausage.obj";
	this->models.emplace_back(Model(&sausagePath[0])); // Sausage ingredient
	g_scene.getEntity("sausage")->attachComponent(&(this->models[6]), "model");

	// Tomato
	std::string tomatoPath = "resources/models/ingredients/tomato.obj";
	this->models.emplace_back(Model(&tomatoPath[0])); // Tomato ingredient
	g_scene.getEntity("tomato")->attachComponent(&(this->models[7]), "model");

	// Dough
	std::string doughPath = "resources/models/ingredients/dough.obj";
	this->models.emplace_back(Model(&doughPath[0])); // Dough ingredient
	g_scene.getEntity("dough")->attachComponent(&(this->models[8]), "model");

	//-----------------------------------------------------------------------------------
	// Debug models
	//-----------------------------------------------------------------------------------
	std::string testPath = "resources/models/ball/ball.obj";
	this->models.emplace_back(Model(&testPath[0]));
	g_scene.getEntity("test")->attachComponent(&(this->models[9]), "model");
}

void RenderingSystem::setupCameras(Transform* player1Transform)
{
	// Get view matrix from Camera and update the Shader
	glm::mat4 view = g_scene.camera.getViewMatrix(player1Transform);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	// Tell shader about camera position
	unsigned int viewPosLoc = glGetUniformLocation(getShaderId(), "viewPos");
	glUniform3f(viewPosLoc, 0, 10.0f, -50.0f);

	// Projection matrix will be handled by the Camera class in the future
	glm::mat4 proj = glm::mat4(1.0f);
	proj = glm::perspective(glm::radians(g_scene.camera.getPerspective()), 800.0f / 600.0f, 0.1f, 1000.0f);
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));
}

void RenderingSystem::update()
{
	// 1. Render depth of scene to texture (from light's perspective)
	glm::mat4 lightProjection, lightView, lightSpaceMatrix;
	lightProjection = glm::ortho(ort.x, ort.y, ort.z, ort.w, ort.nearPlane, ort.farPlane);
	Transform* lightTrans = g_scene.getEntity("test")->getTransform();
	lightView = glm::lookAt(lightTrans->position, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	lightSpaceMatrix = lightProjection * lightView;

	// Render scene from light's point of view
	depthShader.use();
	depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

	glViewport(0, 0, 1024, 1024);
	glBindFramebuffer(GL_FRAMEBUFFER, this->depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	renderScene(depthShader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Reset viewport
	glViewport(0, 0, g_systems.width, g_systems.height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render depthMap to quad for visual debugging
	depthShader.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->depthMapTex);
	renderQuad();
}

Model* RenderingSystem::getKitchenModel()
{
	return g_scene.getEntity("countertop")->getModel();
}

void RenderingSystem::renderScene(Shader &shader)
{
	// Get model unfirom location in shader
	unsigned int modelLoc = glGetUniformLocation(shader.getId(), "model");

	// Render scene to depthMapFBO
	for (auto it = this->models.begin(); it < this->models.end(); it++)
	{
		// Update model matrix
		Transform* ownerTransform = it->owner->getTransform();
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(ownerTransform->getModelMatrix()));

		it->drawDepthMap(shader);
	}
}

void RenderingSystem::renderQuad()
{
	// Draw to default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, g_systems.width, g_systems.height);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render scene to viewport by applying depthMap as texture to 2D quad
	this->debugShader.use();
	glBindVertexArray(this->quadVAO);
	glBindTexture(GL_TEXTURE_2D, this->depthMapTex);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}