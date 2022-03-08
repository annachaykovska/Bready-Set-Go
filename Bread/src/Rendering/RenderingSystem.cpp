#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

#include "../SystemManager.h"
#include "RenderingSystem.h"
#include "../Scene/Scene.h"
#include "../Scene/Entity.h"
#include "GroundModel.h"

extern Scene g_scene;
extern SystemManager g_systems;

RenderingSystem::RenderingSystem(DebugOverlay& debug)
	: shader("resources/shaders/vertex.txt", "resources/shaders/fragment.txt")
	, lightShader("resources/shaders/lightSourceVertex.txt", "resources/shaders/lightSourceFragment.txt")
	, borderShader("resources/shaders/lightSourceVertex.txt", "resources/shaders/borderFragment.txt")
	, simpleShader("resources/shaders/simpleVertex.txt", "resources/shaders/simpleFragment.txt")
	, debugOverlay(debug)
{
	this->screenWidth = 0;
	this->screenHeight = 0;

	this->models.reserve(g_scene.count()); // Create space for models

	// Rendering uniforms
	this->modelLoc = glGetUniformLocation(getShaderId(), "model");
	this->texLoc = glGetUniformLocation(getShaderId(), "textured");
	this->viewLoc = glGetUniformLocation(getShaderId(), "view");
	this->projLoc = glGetUniformLocation(getShaderId(), "proj");

	glEnable(GL_DEPTH_TEST); // Turn on depth testing
	glDepthFunc(GL_LESS);

	glEnable(GL_STENCIL_TEST); // Turn on stencil testing
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	loadModels(); // Load model files into the models vector

	// Camera
	Transform transform = Transform();
	transform.position = glm::vec3(1.0f);
	setupCameras(&transform); // Setup the camera

	// Framebuffer
	glGenFramebuffers(1, &this->FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &this->textureColorBuffer);
	glBindTexture(GL_TEXTURE_2D, this->textureColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, g_systems.width, g_systems.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Attach textureColorBuffer to FBO
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->textureColorBuffer, 0);

	// Renderbuffer
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, g_systems.width, g_systems.height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// Attach RBO to framebuffer's depth and stencil attachment 
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER::Framebuffer is not complete!\n";

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
	if (this->screenWidth != g_systems.width || this->screenHeight != g_systems.height)
	{
		glBindTexture(GL_TEXTURE_2D, this->textureColorBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, g_systems.width, g_systems.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);

		glBindRenderbuffer(GL_RENDERBUFFER, RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, g_systems.width, g_systems.height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	// Bind to FBO to render scene to texture
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glEnable(GL_DEPTH_TEST);

	// Clear frame
	glClearColor(0.6784f, 0.8471f, 0.902f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// Stencil buffer
	glStencilMask(0x00); // Turn off writing to stencil buffer

	// Declare shader to use
	shader.use();

	// Update camera
	Transform* p1Transform = g_scene.getEntity("player1")->getTransform();
	g_scene.camera.updateCameraVectors(p1Transform);
	setupCameras(p1Transform);

	// Turn off textures - temporary
	glUniform1i(texLoc, 0);
	for (toggleDebugView view : debugOverlay.debugMeshes())
	{
		if (view.second)
		{
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Transform().getModelMatrix()));
			glUniform1i(texLoc, 0); // Turn textures off
			view.first.setWireframe(true);
			view.first.draw(getShader());
		}
	}

	glUniform1i(this->texLoc, 0);
	
	// Iterate through all the models in the scene and render them at their new transforms
	for (int i = 0; i < models.size(); i++)
	{
		Transform* ownerTransform = models[i].owner->getTransform();

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(ownerTransform->getModelMatrix()));

		if (i == 4)
			ownerTransform->update();

		if (i <= 4)
		{
			glUniform1i(texLoc, 0);
			models[i].draw(this->shader);
		}
		else if (i > 4 && i <= 8) // Use textures images for ingredients
		{
			glUniform1i(texLoc, 1);
			models[i].draw(this->shader);
		}
		else if (i > 8)
		{
			// DRAW LIGHT BALL
			
			// Stencil buffer
			glStencilFunc(GL_ALWAYS, 1, 0xFF); // Always pass stencil test, ref value, AND stencil buffer with 1
			glStencilMask(0xFF); // Allow writing to stencil buffer

			lightShader.use();

			glBindVertexArray(models[i].meshes[0].VAO);
			glBindBuffer(GL_ARRAY_BUFFER, models[i].meshes[0].VBO);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

			// Load the indice data into the EBO
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, models[i].meshes[0].EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, models[i].meshes[0].indices.size() * sizeof(unsigned int), &models[i].meshes[0].indices[0], GL_STATIC_DRAW);

			// Rendering uniforms
			unsigned int modelLoc = glGetUniformLocation(this->lightShader.getId(), "model");
			unsigned int viewLoc = glGetUniformLocation(this->lightShader.getId(), "view");
			unsigned int projLoc = glGetUniformLocation(this->lightShader.getId(), "projection");

			Transform* t = g_scene.getEntity("test")->getTransform();
			ownerTransform->update();
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(ownerTransform->getModelMatrix()));

			glm::mat4 view = g_scene.camera.getViewMatrix(p1Transform);
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

			glm::mat4 proj = glm::mat4(1.0f);
			proj = glm::perspective(glm::radians(g_scene.camera.getPerspective()), 800.0f / 600.0f, 0.1f, 1000.0f);
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));
			
			glDrawElements(GL_TRIANGLES, models[i].meshes[0].indices.size(), GL_UNSIGNED_INT, 0);

			// DRAW OUTLINE FOR LIGHT BALL

			glStencilFunc(GL_NOTEQUAL, 1, 0xFF); // Pass if not equal to 1, AND stencil buffer with 1
			glStencilMask(0x00); //	Sets passing 
			glDisable(GL_DEPTH_TEST);
			
			borderShader.use();
			
			glBindVertexArray(models[i].meshes[0].VAO);
			glBindBuffer(GL_ARRAY_BUFFER, models[i].meshes[0].VBO);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

			modelLoc = glGetUniformLocation(this->borderShader.getId(), "model");
			viewLoc = glGetUniformLocation(this->borderShader.getId(), "view");
			projLoc = glGetUniformLocation(this->borderShader.getId(), "projection");

			t = g_scene.getEntity("test")->getTransform();
			t->scale = glm::vec3(1.2f);
			ownerTransform->update();

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(ownerTransform->getModelMatrix()));

			view = g_scene.camera.getViewMatrix(p1Transform);
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

			proj = glm::mat4(1.0f);
			proj = glm::perspective(glm::radians(g_scene.camera.getPerspective()), 800.0f / 600.0f, 0.1f, 1000.0f);
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));
			
			glDrawElements(GL_TRIANGLES, models[i].meshes[0].indices.size(), GL_UNSIGNED_INT, 0);

			glStencilMask(0xFF);
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glEnable(GL_DEPTH_TEST);

			t->scale = glm::vec3(1.0f);

			glBindVertexArray(0);
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	simpleShader.use();
	glBindVertexArray(this->quadVAO);
	glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

Model* RenderingSystem::getKitchenModel()
{
	return g_scene.getEntity("countertop")->getModel();
}