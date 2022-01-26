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
#include "Audio/Audio.h"
#include "Audio/AudioSource.h"
#include "Controls.cpp"

const float radius = 40.0f;

int main()
{
	// Initialize GLFW library
	glfwInit();

	// Create viewport window
	Window window(800, 600, "Bready Set Go!!!");
	window.setBackgroundColor(0.6784f, 0.8471f, 0.902f);

	glEnable(GL_DEPTH_TEST); // This should go somewhere else later

	// Compile shader program
	Shader shader("resources/shaders/vertex.txt", "resources/shaders/fragment.txt");

	// ImGui profiler for debugging
	Profiler profiler(window);

	// Load breadbus model
	std::string busPath = "resources/models/breadbus/breadbus.obj";
	Model bus(&busPath[0]);
	bus.meshes[0].color = glm::vec3(0.8f, 0.467381f, 0.072319f); // Face
	bus.meshes[1].color = glm::vec3(0.366761f, 0.134041f, 0); // Crust
	bus.meshes[2].color = glm::vec3(0.75f, 0.85f, 0.85f); // Windows
	bus.meshes[3].color = glm::vec3(1, 0.89f, 0.71f); // Headlights
	bus.meshes[4].color = glm::vec3(0.1f, 0.1f, 0.1f); // Front wheels
	//bus.meshes[5].color = glm::vec3(0.266761f, 0.034041f, 0); // Dunno!
	bus.meshes[5].color = glm::vec3(1, 0, 1);
	//bus.meshes[6].color = glm::vec3(0.8f, 0.467381f, 0.072319f); // Dunno!
	bus.meshes[6].color = glm::vec3(1, 0, 1);
	//bus.meshes[7].color = glm::vec3(0.8f, 0.039734f, 0); // Dunno!
	bus.meshes[7].color = glm::vec3(1, 0, 1);
	bus.meshes[8].color = glm::vec3(0.1f, 0.1f, 0.1f); // Back wheels

	// Load teapot model
	std::string teapotPath = "resources/models/teapot/teapot_s0.obj";
	Model teapot(&teapotPath[0]);
	teapot.meshes[0].color = glm::vec3(0.3f, 0.3f, 0.3f);

	// Create ground plane
	Geometry geo;
	Mesh ground = geo.createGround();

	// MVP uniforms
	unsigned int modelLoc = glGetUniformLocation(shader.getId(), "model");
	unsigned int viewLoc = glGetUniformLocation(shader.getId(), "view");
	unsigned int projLoc = glGetUniformLocation(shader.getId(), "proj");

	// For detecting textures
	unsigned int texLoc = glGetUniformLocation(shader.getId(), "textured");

	// Create a contatiner for transform components (will be handled by systems in future)
	std::vector<std::shared_ptr<Transform>> transforms;

	// Create entities and initialize their transforms
	Entity breadBus;
	transforms.push_back(std::make_shared<Transform>());
	transforms[0]->position = glm::vec3(0, 2.3f, 0);
	transforms[0]->rotation = glm::vec3(0, 0, 0);
	transforms[0]->scale = glm::vec3(3, 3, 3);
	breadBus.addComponent(transforms[0]);

	Entity tea;
	transforms.push_back(std::make_shared<Transform>());
	transforms[1]->position = glm::vec3(50, 0, 50);
	transforms[1]->rotation = glm::vec3(0, 0, 0);
	transforms[1]->scale = glm::vec3(50, 50, 50);
	tea.addComponent(transforms[1]);

	Entity countertop;
	transforms.push_back(std::make_shared<Transform>());
	transforms[2]->position = glm::vec3(0, 0, 0);
	transforms[2]->rotation = glm::vec3(0, 0, 0);
	transforms[2]->scale = glm::vec3(100, 100, 100);
	countertop.addComponent(transforms[2]);

	// Initialize physx
	PhysicsSystem physics;
	float oldTime = glfwGetTime(), newTime = 0, deltaTime = 0;

	// Set movement control callbacks
	// TODO: generalize this
	auto movementCallbacks = std::make_shared<MovementCallbacks>(transforms[0]); 
	window.setCallbacks(movementCallbacks);


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
		shader.use();
		profiler.newFrame();

		// View matrix will be handled by the Camera class in the future
		glm::mat4 view = glm::mat4(1.0f);
		float camX = sin(glfwGetTime()) * radius;
		float camZ = cos(glfwGetTime()) * radius;
		view = glm::lookAt(glm::vec3(camX, 10.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		// Projection matrix will be handled by the Camera class in the future
		glm::mat4 proj = glm::mat4(1.0f);
		proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 1000.0f);
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

		// Drawing objects will be handled by Rendering System in the future
		// Draw breadbus
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(transforms[0]->getModelMatrix()));
		glUniform1i(texLoc, 0); // Turn off textures
		bus.draw(shader);

		// Draw teapot
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(transforms[1]->getModelMatrix()));
		teapot.draw(shader);

		// Draw countertop
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(transforms[2]->getModelMatrix()));
		glUniform1i(texLoc, 1); // Turn textures back on
		ground.draw(shader);

		// Update the ImGUI profiler
		profiler.update(transforms[0]->position);

		// Swap the frame buffers
		window.swapBuffer();

		// AUDIO
		//testSource.setPosition(glm::vec3(camX / 2, 0, camZ / 2));
	}

	// Collect garbage
	profiler.cleanup();
	shader.cleanup();
	glfwTerminate();

	return 0;
}
