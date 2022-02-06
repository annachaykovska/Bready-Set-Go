#include <iostream>
#include <vector>

#include "Window.h"
#include "Profiler.h"
#include "SystemManager.h"
#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Transform.h"
#include "Rendering/RenderingSystem.h"
#include "Audio/AudioSystem.h"
#include "Physics/VehicleController.cpp"
//#include "Controls.cpp"

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

	//-----------------------------------------------------------------------------------
	// ENTITY-COMPONENT STUFF 
    //-----------------------------------------------------------------------------------
	// Populate the scene with Entities (will happen on game start)
	// This happens all at once to avoid dangling pointers in the future
	g_scene.createEntities();

	// Get references to Entities for easy use and set vehicle physx objects
	Entity* player1 = g_scene.getEntity("player1");
	Entity* player2 = g_scene.getEntity("player2");
	Entity* countertop = g_scene.getEntity("countertop");

	//-----------------------------------------------------------------------------------
	// INITIALIZE SYSTEMS
	//-----------------------------------------------------------------------------------
	PhysicsSystem physics;
	g_systems.physics = &physics;
	player1->vehicle = physics.mVehicle4W; // Don't think this is needed?

	RenderingSystem renderer;
	g_systems.render = &renderer;

	AudioSystem audio;
	g_systems.audio = &audio;

	//-----------------------------------------------------------------------------------
	// INITIALIZE TRANSFORMS - will be handled by PhysicsSystem eventually
	//-----------------------------------------------------------------------------------
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

	// Initialize transform components
	Transform* aComponent = (Transform*)player1->getComponent("transform");
	Transform* player1Transform = player1->getTransform();
	player1Transform->position = glm::vec3(-10, 2.3f, 3);
	player1Transform->rotation = glm::vec3(0, 0, 0);
	player1Transform->scale = glm::vec3(3, 3, 3);

	Transform* player2Transform = player2->getTransform();
	player2Transform->position = glm::vec3(10, 0, -3);
	player2Transform->rotation = glm::vec3(0, 90, 0);
	player2Transform->scale = glm::vec3(3, 3, 3);

	Transform* counterTransform = countertop->getTransform();
	counterTransform->position = glm::vec3(0, -10, 0);
	counterTransform->rotation = glm::vec3(0, 0, 0);
	counterTransform->scale = glm::vec3(50, 50, 50);
	//-----------------------------------------------------------------------------------

	// Get a reference to the countertop's AudioSource to play background music
	// Leaving this here for now so people can turn the music on/off easily
	AudioSource* countertopAudioSource = countertop->getAudioSource();
	//countertopAudioSource->play("bg.wav");

	// Track time
	float oldTime = glfwGetTime(), newTime = 0, deltaTime = 0;

	// Set movement control callbacks
	// TODO: generalize this
	auto movementCallbacks = std::make_shared<MovementCallbacks>(&physics); 
	window.setCallbacks(movementCallbacks);

	// GAME LOOP
	while (!window.shouldClose())
	{
		// INPUT
		glfwPollEvents();

		// SIMULATE
		newTime = glfwGetTime();
		deltaTime = newTime - oldTime;
		g_systems.physics->update(deltaTime);
		oldTime = newTime;

		// RENDER
		window.clear();
		renderer.update();
		profiler.newFrame();

		// Update the ImGUI profiler
		profiler.update();

		// Swap the frame buffers
		window.swapBuffer();

		// AUDIO
		// update AudioSource
	}

	// Collect garbage
	physics.cleanupPhysics();
	profiler.cleanup();
	renderer.getShader().cleanup();
	glfwTerminate();

	return 0;
}