#include <glm/gtc/type_ptr.hpp>

#include "RenderingSystem.h"
#include "../Scene/Scene.h"
#include "../Scene/Entity.h"
#include "GroundModel.h"

extern Scene g_scene;

RenderingSystem::RenderingSystem() : shader("resources/shaders/vertex.txt", "resources/shaders/fragment.txt")
{
	std::string breadmobilePath = "resources/models/breadbus/breadbus2.obj";
	player1 = Model(&breadmobilePath[0]);
	std::cout << "After player1" << std::endl;
	player2 = Model(&breadmobilePath[0]);
	countertop = Model(GroundModel::createGround());

	glEnable(GL_DEPTH_TEST);

	this->models.reserve(g_scene.count());

	this->modelLoc = glGetUniformLocation(getShaderId(), "model");
	this->texLoc = glGetUniformLocation(getShaderId(), "textured");

	this->viewLoc = glGetUniformLocation(getShaderId(), "view");
	this->projLoc = glGetUniformLocation(getShaderId(), "proj");

	loadModels();
	setupCameras();
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
	
	// Load breadmobile models
	std::string breadmobilePath = "resources/models/breadbus/breadbus2.obj";
	//models.push_back(Model(&breadmobilePath[0]));
	//models.push_back(Model(&breadmobilePath[0]));

	// Create ground plane
	//models.push_back(GroundModel::createGround());

	Entity* player1 = g_scene.getEntity("player1");
	Entity* player2 = g_scene.getEntity("player2");
	Entity* countertop = g_scene.getEntity("countertop");

	player1->attachComponent(&(this->player1), "model");
	player2->attachComponent(&(this->player2), "model");
	countertop->attachComponent(&(this->countertop), "model");
}

void RenderingSystem::setupCameras()
{
	// View matrix will be handled by the Camera class in the future
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::lookAt(glm::vec3(0, 10.0f, -50.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
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
	Entity* player1 = g_scene.getEntity("player1");
	Entity* player2 = g_scene.getEntity("player2");
	Entity* countertop = g_scene.getEntity("countertop");

	Model* player1Model = player1->getModel();
	Model* player2Model = player2->getModel();
	Model* countertopModel = countertop->getModel();

	Transform* player1Transform = player1->getTransform();
	Transform* player2Transform = player2->getTransform();
	Transform* countertopTransform = countertop->getTransform();

	shader.use();
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
	
	// Draw player1 with new transform
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(player1Transform->getModelMatrix()));
	glUniform1i(texLoc, 0); // Turn off textures
	player1Model->draw(getShader());

	// Draw player2 with new transform
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(player2Transform->getModelMatrix()));
	player2Model->draw(getShader());

	// Draw countertop with new transform
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(countertopTransform->getModelMatrix()));
	glUniform1i(texLoc, 1); // Turn textures back on
	countertopModel->draw(getShader());
}