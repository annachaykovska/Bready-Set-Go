#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

#include "RenderingSystem.h"
#include "../Scene/Scene.h"
#include "../Scene/Entity.h"
#include "GroundModel.h"

extern Scene g_scene;

RenderingSystem::RenderingSystem() : shader("resources/shaders/vertex.txt", "resources/shaders/fragment.txt")
{
	std::string breadmobilePath = "resources/models/breadbus/breadbus2.obj";
	this->player1 = Model(&breadmobilePath[0]);
	
	std::string groundPath = "resources/models/ground/roughGround.obj";
	this->countertop = Model(&groundPath[0]);

	this->player2 = Model(&breadmobilePath[0]);

	glEnable(GL_DEPTH_TEST);

	this->models.reserve(g_scene.count());

	this->modelLoc = glGetUniformLocation(getShaderId(), "model");
	this->texLoc = glGetUniformLocation(getShaderId(), "textured");

	this->viewLoc = glGetUniformLocation(getShaderId(), "view");
	this->projLoc = glGetUniformLocation(getShaderId(), "proj");

	loadModels();
	setupCameras();
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
	// Load breadmobile models
	std::string breadmobilePath = "resources/models/breadbus/breadbus2.obj";
	//models.push_back(Model(&breadmobilePath[0]));
	//models.push_back(Model(&breadmobilePath[0]));

	// Create ground plane
	//models.push_back(GroundModel::createGround());

	Entity* p1 = g_scene.getEntity("player1");
	Entity* p2 = g_scene.getEntity("player2");
	Entity* ground = g_scene.getEntity("countertop");

	if (p1->attachComponent(&(this->player1), "model"))
		std::cout << "Player1 model attached succesfully!\n";
	if (p2->attachComponent(&(this->player2), "model"))
		std::cout << "Player2 model attached succesfully!\n";
	if (ground->attachComponent(&(this->countertop), "model"))
		std::cout << "Countertop model attached succesfully!\n";
}

void RenderingSystem::setupCameras()
{
	// Get view matrix from Camera and update the Shader
	glm::mat4 view = g_scene.camera.getViewMatrix();
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	// Tell shader about camera position
	unsigned int viewPosLoc = glGetUniformLocation(getShaderId(), "viewPos");
	glUniform3f(viewPosLoc, 0, 10.0f, -50.0f);

	// Projection matrix will be handled by the Camera class in the future
	glm::mat4 proj = glm::mat4(1.0f);
	proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 1000.0f);
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));
}

void RenderingSystem::update()
{
	// Get references to things that need to be updated
	Entity* p1 = g_scene.getEntity("player1");
	Entity* p2 = g_scene.getEntity("player2");
	Entity* ground = g_scene.getEntity("countertop");

	Model* p1Model = p1->getModel();
	Model* p2Model = p2->getModel();
	Model* groundModel = ground->getModel();

	Transform* p1Transform = p1->getTransform();
	Transform* p2Transform = p2->getTransform();
	Transform* groundTransform = ground->getTransform();

	shader.use();
	g_scene.camera.updateCameraVectors();
	setupCameras();

	//glUniform1i(texLoc, 0); // Turn off textures

	// Iterate through all the models in the scene and render them at their new transforms
	/*
	for (int i = 0; i < models.size(); i++)
	{
		Entity* ownerEntity = models[i].owner;
		glm::mat4 modelMatrix = ownerEntity->getTransform()->getModelMatrix();
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		models[i].draw(getShader());
	}*/
	
	glUniform1i(texLoc, 0); // Turn off textures

	//std::cout << "Player1: (" << p1Transform->position.x << ", " << p1Transform->position.y
	//		  << ", " << p1Transform->position.z << ")\n";

	//std::cout << "Player2: (" << p2Transform->position.x << ", " << p2Transform->position.y
	//		  << ", " << p2Transform->position.z << ")\n";

	// Draw player1 with new transform
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(p1Transform->getModelMatrix()));
	p1Model->draw(getShader());

	// Draw player2 with new transform
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(p2Transform->getModelMatrix()));
	p2Model->draw(getShader());

	// Draw countertop with new transform
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(groundTransform->getModelMatrix()));
	groundModel->draw(getShader());
}