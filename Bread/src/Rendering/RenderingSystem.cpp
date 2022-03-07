#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

#include "RenderingSystem.h"
#include "../Scene/Scene.h"
#include "../Scene/Entity.h"
#include "GroundModel.h"

extern Scene g_scene;

RenderingSystem::RenderingSystem() : shader("resources/shaders/vertex.txt", "resources/shaders/fragment.txt"),
									 lightShader("resources/shaders/lightSourceVertex.txt", "resources/shaders/lightSourceFragment.txt")
{
	this->models.reserve(g_scene.count()); // Create space for models

	// Rendering uniforms
	this->modelLoc = glGetUniformLocation(getShaderId(), "model");
	this->texLoc = glGetUniformLocation(getShaderId(), "textured");
	this->viewLoc = glGetUniformLocation(getShaderId(), "view");
	this->projLoc = glGetUniformLocation(getShaderId(), "proj");

	glEnable(GL_DEPTH_TEST); // Turn on depth testing

	loadModels(); // Load model files into the models vector

	// Camera
	Transform transform = Transform();
	transform.position = glm::vec3(1.0f);
	setupCameras(&transform); // Setup the camera
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
	// Declare shader to use
	shader.use();

	// Update camera
	Transform* p1Transform = g_scene.getEntity("player1")->getTransform();
	g_scene.camera.updateCameraVectors(p1Transform);
	setupCameras(p1Transform);

	// Turn off textures - temporary
	glUniform1i(this->texLoc, 0);
	
	// Iterate through all the models in the scene and render them at their new transforms
	for (int i = 0; i < models.size(); i++)
	{
		Transform* ownerTransform = models[i].owner->getTransform();

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(ownerTransform->getModelMatrix()));

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
			lightShader.use();

			glBindVertexArray(models[i].meshes[0].VAO);
			glBindBuffer(GL_ARRAY_BUFFER, models[i].meshes[0].VBO);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

			// Load the indice data into the EBO
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, models[i].meshes[0].EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, models[i].meshes[0].indices.size() * sizeof(unsigned int), &models[i].meshes[0].indices[0], GL_STATIC_DRAW);

			// Rendering uniforms
			unsigned int modelLoc = glGetUniformLocation(this->lightShader.getId(), "model");
			unsigned int viewLoc = glGetUniformLocation(this->lightShader.getId(), "view");
			unsigned int projLoc = glGetUniformLocation(this->lightShader.getId(), "projection");

			ownerTransform->update();
			//std::cout << "Ball pos: " << "(" << ownerTransform->position.x << ", " << ownerTransform->position.y << ", " << ownerTransform->position.z << ")";
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(ownerTransform->getModelMatrix()));

			glm::mat4 view = g_scene.camera.getViewMatrix(p1Transform);
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

			glm::mat4 proj = glm::mat4(1.0f);
			proj = glm::perspective(glm::radians(g_scene.camera.getPerspective()), 800.0f / 600.0f, 0.1f, 1000.0f);
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));
			
			glDrawElements(GL_TRIANGLES, models[i].meshes[0].indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
	}
}

Model* RenderingSystem::getKitchenModel()
{
	return g_scene.getEntity("countertop")->getModel();
}