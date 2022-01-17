#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <PxPhysicsAPI.h>

#include "Window.h"
#include "Profiler.h"
#include "Geometry.h"
#include "Shader.h"
#include "Model.h"
#include "Entity.h"
#include "Camera.h"
#include "PhysicsSystem.h"

//Camera camera;

int main()
{	
	// Initialize GLFW library
	glfwInit();

	// Create viewport window
	Window window(800, 600, "Bread");
	window.setBackgroundColor(0.2f, 0.3f, 0.3f);

	// Compile shader program
	Shader shader("resources/shaders/vertex.txt", "resources/shaders/fragment.txt");

	// ImGui profiler for debugging
	Profiler profiler(window);

	// Create ground plane
	Geometry geo;
	Mesh ground = geo.createGround();

	// Load bread model
	std::string breadPath = "resources/models/bread/PB145_bread_low.obj";
	Model test(&breadPath[0]);

	const float radius = 40.0f;

	glEnable(GL_DEPTH_TEST);

	// Create new entities
	Entity bread;
	Entity bread2;

	// Creation of components should be handled by their respective system
	std::shared_ptr<Transform> breadTransform = std::make_shared<Transform>();
	breadTransform->position = glm::vec3(1.0f, 2.0f, 3.0f);
	breadTransform->rotation = glm::vec3(45.0f, 90.0f, 180.0f);
	breadTransform->scale = glm::vec3(1.0f, 1.0f, 0.8f);

	// Add a new component to an entity
	bread.addComponent(breadTransform);

	// Initialize physx
	PhysicsSystem physics;

	float oldTime = glfwGetTime(), newTime = 0, deltaTime = 0;

	// GAME LOOP
	while (!window.shouldClose()) 
	{
		// INPUT
		window.getInput();
		glfwPollEvents();

		// SIMULATE
		newTime = glfwGetTime();
		deltaTime = newTime - oldTime;
		oldTime = newTime;
		physics.update(deltaTime);

		// RENDER
		window.clear();
		profiler.update();
		shader.use();

		// Model matrix will be handled by Transform components in the future
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 5.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-70.00f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));

		// View matrix will be handled by Camera class in the future
		glm::mat4 view = glm::mat4(1.0f);
		float camX = sin(glfwGetTime()) * radius;
		float camZ = cos(glfwGetTime()) * radius;
		view = glm::lookAt(glm::vec3(camX, 10.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		// Projection matrix will be handled by Camera class in the future
		glm::mat4 proj = glm::mat4(1.0f);
		proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 1000.0f);

		// Used to turn textures on/off for specific meshes
		unsigned int texLoc = glGetUniformLocation(shader.getId(), "textured");
		glUniform1i(texLoc, 0);

		// MVP uniforms
		unsigned int modelLoc = glGetUniformLocation(shader.getId(), "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		unsigned int viewLoc = glGetUniformLocation(shader.getId(), "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		unsigned int projLoc = glGetUniformLocation(shader.getId(), "proj");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

		test.draw(shader); // Draw the bread loaf

		// Change model matrix for ground plane
		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(100.0f, 100.0f, 100.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		ground.draw(shader); // Draw the ground

		window.swapBuffer();

		// AUDIO
	}

	profiler.cleanup();
	shader.cleanup();
	glfwTerminate();

	return 0;
}