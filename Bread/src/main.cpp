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

const float radius = 40.0f;

int main()
{	
	// Initialize GLFW library
	glfwInit();

	// Create viewport window
	Window window(800, 600, "Bready Set Go!!!");
	window.setBackgroundColor(0.2f, 0.3f, 0.3f);

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
	bus.meshes[8].color = glm::vec3(0.1f, 0.1f, 0.1f); // Back wheels*/

	// Create ground plane
	Geometry geo;
	Mesh ground = geo.createGround();

	// Create new entities
	Entity bread;

	// Creation of components should be handled by their respective system in the future
	std::shared_ptr<Transform> breadTransform = std::make_shared<Transform>();
	breadTransform->position = glm::vec3(1.0f, 2.0f, 3.0f);
	breadTransform->rotation = glm::vec3(45.0f, 90.0f, 180.0f);
	breadTransform->scale = glm::vec3(1.0f, 1.0f, 0.8f);

	// Add a new component to an entity
	bread.addComponent(breadTransform);

	// Initialize physx
	PhysicsSystem physics;

	// Play the background music
	Audio audioTest;
	AudioSource testSource(0.1f, 1.0f, true);
	testSource.play(audioTest.get("bg.wav"));

	// MVP uniforms
	unsigned int modelLoc = glGetUniformLocation(shader.getId(), "model");
	unsigned int viewLoc = glGetUniformLocation(shader.getId(), "view");
	unsigned int projLoc = glGetUniformLocation(shader.getId(), "proj");

	// For detecting textures
	unsigned int texLoc = glGetUniformLocation(shader.getId(), "textured");

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

		// View matrix will be handled by Camera class in the future
		glm::mat4 view = glm::mat4(1.0f);
		float camX = sin(glfwGetTime()) * radius;
		float camZ = cos(glfwGetTime()) * radius;
		view = glm::lookAt(glm::vec3(camX, 10.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		// Projection matrix will be handled by Camera class in the future
		glm::mat4 proj = glm::mat4(1.0f);
		proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 1000.0f);
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));		

		// Draw breadbus
		glm::mat4 model = glm::mat4(1.0f); // Model matrix will be handled by transform components in the future
		model = glm::translate(model, glm::vec3(0.0f, 2.3f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1i(texLoc, 0); // Turn off textures
		bus.draw(shader);

		// Draw countertop
		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(100.0f, 100.0f, 100.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1i(texLoc, 1); // Turn textures back on
		ground.draw(shader); 

		// Swap the frame buffers
		window.swapBuffer();

		// AUDIO
		// Doesn't need to be updated every frame yet
	}

	// Collect garbage
	profiler.cleanup();
	shader.cleanup();
	glfwTerminate();

	return 0;
}