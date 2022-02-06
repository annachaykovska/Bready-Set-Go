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
#include "SystemManager.h"
#include "Scene/Scene.h"
#include "Geometry.h"
#include "Shader.h"
#include "Model.h"
#include "Camera.h"
#include "Physics/VehicleController.cpp"

// Global Scene holds all the Entities for easy reference
Scene g_scene;

// Global SystemManager holds all the Systems for easy reference
SystemManager g_systems;

int main()
{
	// Initialize GLFW library
	glfwInit();

	// Create viewport window
	Window window(800, 600, "Bready Set Go!!!");
	window.setBackgroundColor(0.6784f, 0.8471f, 0.902f);

	// ImGui profiler for debugging
	Profiler profiler(window);

	// Initialize physics System
	PhysicsSystem physics;
	g_systems.physics = &physics;

	//-----------------------------------------------------------------------------------
	// INITIALIZE RENDERING STUFF - will be in the rendering system in the future
	//-----------------------------------------------------------------------------------
	// Compile shader program
	Shader shader("resources/shaders/vertex.txt", "resources/shaders/fragment.txt");

	// Turn on depth testing so we know what objects are in front of what
	glEnable(GL_DEPTH_TEST);

	// Load breadbus model
	std::string breadmobilePath = "resources/models/breadbus/breadbus2.obj";
	Model breadmobile(&breadmobilePath[0]);
	breadmobile.meshes[0].color = glm::vec3(0.8f, 0.467381f, 0.072319f); // Face
	breadmobile.meshes[1].color = glm::vec3(0.366761f, 0.134041f, 0); // Crust
	breadmobile.meshes[2].color = glm::vec3(0.75f, 0.85f, 0.85f); // Windows
	breadmobile.meshes[3].color = glm::vec3(1, 0.89f, 0.71f); // Headlights
	breadmobile.meshes[4].color = glm::vec3(0.1f, 0.1f, 0.1f); // Front wheels
	breadmobile.meshes[5].color = glm::vec3(0.1f, 0.1f, 0.1f); // Back wheels

	// Create ground plane
	Geometry geo;
	Mesh ground = geo.createGround();

	Mesh navDebug;
	Vertex nav0, nav1, nav2;
	nav0.position = glm::vec3(10.0f, 1.0f, 10.0f);
	nav0.normal = glm::vec3(0.0f, 1.0f, 0.0f);
	nav1.position = glm::vec3(10.0f, 1.0f, 0.0f);
	nav1.normal = glm::vec3(0.0f, 1.0f, 0.0f);
	nav2.position = glm::vec3(0.0f, 1.0f, 0.0f);
	nav2.normal = glm::vec3(0.0f, 1.0f, 0.0f);

	navDebug.vertices.push_back(nav0);
	navDebug.vertices.push_back(nav1);
	navDebug.vertices.push_back(nav2);

	navDebug.indices.push_back(0);
	navDebug.indices.push_back(1);
	navDebug.indices.push_back(2);

	navDebug.setWireframe(true);

	navDebug.setupMesh();

	// MVP uniforms
	unsigned int modelLoc = glGetUniformLocation(shader.getId(), "model");
	unsigned int viewLoc = glGetUniformLocation(shader.getId(), "view");
	unsigned int projLoc = glGetUniformLocation(shader.getId(), "proj");

	// For detecting textures
	unsigned int texLoc = glGetUniformLocation(shader.getId(), "textured");

	//-----------------------------------------------------------------------------------
	// ENTITY-COMPONENT STUFF
	//-----------------------------------------------------------------------------------
	// Populate the scene with Entities (will happen on game start)
	// This happens all at once to avoid dangling pointers in the future
	g_scene.createEntities();

	// Get references to Entities for easy use and set vehicle physx objects
	Entity* player1 = g_scene.getEntity("player1");
	player1->vehicle = physics.mVehicle4W; 
	Entity* player2 = g_scene.getEntity("player2");
	Entity* countertop = g_scene.getEntity("countertop");

	Transform navMeshTransform;
	navMeshTransform.position = glm::vec3(0, 0, 0);
	navMeshTransform.rotation = glm::vec3(0, 0, 0);
	navMeshTransform.scale = glm::vec3(1, 1, 1);

	// Create a container for Transform Components (will be handled by a system in the future)
	// and add some some new Transforms to it for the Entities
	std::vector<Transform> transforms;
	transforms.emplace_back(Transform());
	transforms.emplace_back(Transform());
	transforms.emplace_back(Transform());
	
	// Attach one of the Transform Components to each Entity in the Scene
	player1->attachComponent(&transforms[0], "transform");
	player2->attachComponent(&transforms[1], "transform");
	countertop->attachComponent(&transforms[2], "transform");

	// Make a reference to each Transform Component for easy updates
	Transform* aComponent = (Transform*)player1->getComponent("transform");
	Transform* player1Transform = player1->getTransform();
	player1Transform->position = glm::vec3(0, 0, 0);
	player1Transform->rotation = glm::vec3(0, 0, 0);
	player1Transform->scale = glm::vec3(3, 3, 3);

	Transform* player2Transform = player2->getTransform();
	player2Transform->position = glm::vec3(10, 2.3f, -3);
	player2Transform->rotation = glm::vec3(0, 90, 0);
	player2Transform->scale = glm::vec3(3, 3, 3);
	
	Transform* counterTransform = countertop->getTransform();
	counterTransform->position = glm::vec3(0, 0, 0);
	counterTransform->rotation = glm::vec3(0, 0, 0);
	counterTransform->scale = glm::vec3(100, 100, 100);
	//-----------------------------------------------------------------------------------

	// Initialize audio System
	AudioSystem audio;
	g_systems.audio = &audio;

	// Get a reference to the AudioSource Component for easy updates
	AudioSource* countertopAudioSource = countertop->getAudioSource();

	// Tell the AudioSource Component to play a sound file
	//countertopAudioSource->play("bread.wav");

	// Track time
	float oldTime = glfwGetTime(), newTime = 0, deltaTime = 0;

	// Set movement control callbacks
	// TODO: generalize this
	/*std::shared_ptr<Transform> t = std::make_shared<Transform>();
	t->position = player1Transform->position;*/
	auto movementCallbacks = std::make_shared<MovementCallbacks>(&physics); 
	window.setCallbacks(movementCallbacks);

	// GAME LOOP
	while (!window.shouldClose())
	{
		// INPUT
		window.getInput();
		glfwPollEvents();
		//player1Transform->position = t->position;

		// SIMULATE
		newTime = glfwGetTime();
		deltaTime = newTime - oldTime;
		oldTime = newTime;
		g_systems.physics->update(deltaTime);

		// RENDER
		window.clear();
		shader.use();
		profiler.newFrame();

		// View matrix will be handled by the Camera class in the future
		glm::mat4 view = glm::mat4(1.0f);

		view = glm::lookAt(glm::vec3(0.f, 50.0f, 50.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//view = glm::lookAt(glm::vec3(0.f, 200.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		// Projection matrix will be handled by the Camera class in the future
		glm::mat4 proj = glm::mat4(1.0f);
		proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 1000.0f);
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

		// Drawing objects will be handled by Rendering System in the future
		// Draw player1
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(player1Transform->getModelMatrix()));
		glUniform1i(texLoc, 0); // Turn off textures
		breadmobile.draw(shader);

		// Draw player2
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(player2Transform->getModelMatrix()));
		////teapotModel.draw(shader);
		//breadmobile.draw(shader);

		// Draw countertop
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(counterTransform->getModelMatrix()));
		glUniform1i(texLoc, 1); // Turn textures back on
		ground.draw(shader);
		
		// Draw nav debug mesh
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(navMeshTransform.getModelMatrix()));
		glUniform1i(texLoc, 0); // Turn textures off
		navDebug.draw(shader);


		// Update the ImGUI profiler
		profiler.update(transforms[0].position);

		// Swap the frame buffers
		window.swapBuffer();

		// AUDIO
		// update AudioSource
	}

	// Collect garbage
	physics.cleanupPhysics();
	profiler.cleanup();
	shader.cleanup();
	glfwTerminate();

	return 0;
}
