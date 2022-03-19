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
	// Viewport settings
	this->screenWidth = 0;
	this->screenHeight = 0;

	// Orthographic projection for shadow map settings
	this->ort.left = 0;
	this->ort.right = 0;
	this->ort.bottom = 0;
	this->ort.top = 0;
	this->ort.nearPlane = 0.1f;
	this->ort.farPlane = 700.0f;

	// Directional light position
	this->lightPos = glm::vec3(0.0f, 200.0f, 200.0f);
	this->lightDir = glm::vec3(1.0f, -1.0f, 1.0f);

	// Shadow map viewport size
	this->shadowWidth = g_systems.width * 4;
	this->shadowHeight = g_systems.height * 4;

	this->models.reserve(g_scene.count()); // Create space for models
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
	// Configure depth map FBO
	glGenFramebuffers(1, &this->depthMapFBO);
	
	// Create depth texture
	glGenTextures(1, &this->depthMapTex);
	glActiveTexture(GL_TEXTURE25);
	glBindTexture(GL_TEXTURE_2D, this->depthMapTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, this->shadowWidth, this->shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	// Attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, this->depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->depthMapTex, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Shader configuration
	debugShader.use();
	debugShader.setInt("depthMap", 25);

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
	glm::vec3 cameraPos = g_scene.camera.position;
	glUniform3f(viewPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);

	// Projection matrix will be handled by the Camera class in the future
	glm::mat4 proj = glm::mat4(1.0f);
	float screenWidth = g_systems.width;
	float screenHeight = g_systems.height;
	proj = glm::perspective(glm::radians(g_scene.camera.getPerspective()), screenWidth / screenHeight, 0.1f, 500.0f);
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));
}

void RenderingSystem::updateOrtho()
{
	glm::vec3 p1Pos = g_scene.getEntity("player1")->getTransform()->position;

	this->ort.left = p1Pos.x + 50.0f;
	this->ort.right = p1Pos.x - 50.0f;
	this->ort.bottom = -p1Pos.z - 50.0f;
	this->ort.top = -p1Pos.z + 50.0f;
}

void RenderingSystem::update()
{
	// Render depthMap to texture (from light's perspective)
	glm::mat4 lightProjection, lightView, lightSpaceMatrix;
	updateOrtho();
	lightProjection = glm::ortho(ort.left, ort.right, ort.bottom, ort.top, ort.nearPlane, ort.farPlane);
	lightView = glm::lookAt(this->lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	lightSpaceMatrix = lightProjection * lightView;

	depthShader.use();
	depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

	g_scene.getEntity("countertop")->getTransform()->update();

	glViewport(0, 0, this->shadowWidth, this->shadowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, this->depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT); // Switch to front-face culling to reduce peter-panning of shadows
	renderShadowMap();
	glCullFace(GL_BACK); // Switch back to back-face culling for regular rendering
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Debug code for rendering the depthMap to viewport
	if (g_systems.renderDebug)
		renderDebugShadowMap();
	else
	{
		// Render scene as normal using the generated depth/shadow map
		this->shader.use();
		this->shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
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
	glBindTexture(GL_TEXTURE_2D, this->depthMapTex);
	glUniform1i(glGetUniformLocation(this->debugShader.getId(), "shadowMap"), 25);
	renderTexturedQuad();
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

	// Bind shadow map
	glActiveTexture(GL_TEXTURE25);
	glBindTexture(GL_TEXTURE_2D, this->depthMapTex);
	glUniform1i(glGetUniformLocation(this->shader.getId(), "shadowMap"), 25);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE, 0);

	glUniform1i(texLoc, 1);

	// Iterate through all the models in the scene and render them at their new transforms
	for (int i = 0; i < models.size(); i++)
	{
		Transform* ownerTransform = models[i].owner->getTransform();
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(ownerTransform->getModelMatrix()));

		if (i < 4)
		{
			glUniform1i(texLoc, 0);
			models[i].draw(this->shader);
		}
		else if (i >= 4 && i <= 8) // Use textures images for ingredients and kitchen
		{
			if (i == 4)
				ownerTransform->update();

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
	glBindVertexArray(this->quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}