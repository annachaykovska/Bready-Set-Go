#include <iostream>
#include <vector>

#include "Window.h"
#include "Profiler.h"
#include "SystemManager.h"
#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Transform.h"
#include "Rendering/RenderingSystem.h"
#include "Rendering/UISystem.h"
#include "Audio/AudioSystem.h"
#include "Physics/VehicleController.h"
#include "Inventory.h"

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

	// Get references to Entities for easy use
	Entity* player1 = g_scene.getEntity("player1");
	Entity* player2 = g_scene.getEntity("player2");
	Entity* player3 = g_scene.getEntity("player3");
	Entity* player4 = g_scene.getEntity("player4");
	Entity* cheese = g_scene.getEntity("cheese");
	Entity* sausage = g_scene.getEntity("sausage");
	Entity* tomato = g_scene.getEntity("tomato");
	Entity* dough = g_scene.getEntity("dough");
	Entity* countertop = g_scene.getEntity("countertop");

	//-----------------------------------------------------------------------------------
	// INITIALIZE SYSTEMS
	//-----------------------------------------------------------------------------------
	PhysicsSystem physics;
	g_systems.physics = &physics;
	player1->vehicle = physics.mVehiclePlayer1; // Don't think this is needed?
	player2->vehicle = physics.mVehiclePlayer2; // Don't think this is needed?
	player3->vehicle = physics.mVehiclePlayer3; // Don't think this is needed?
	player4->vehicle = physics.mVehiclePlayer4; // Don't think this is needed?

	RenderingSystem renderer;
	g_systems.render = &renderer;
	g_systems.physics->initialize(); // Needs to happen after renderer loads the models

	AudioSystem audio;
	g_systems.audio = &audio;

	//TODO put UI system here
	UISystem ui;
	g_systems.ui = &ui;

	g_scene.init(&physics);

	//-----------------------------------------------------------------------------------
	// INITIALIZE TRANSFORMS - will be handled by PhysicsSystem eventually
	//-----------------------------------------------------------------------------------
	// Create a container for Transform Components (will be handled by a system in the future)
	// and add some some new Transforms to it for the Entities
	std::vector<Transform> transforms;
	for (int i = 0; i < 9; i++)
		transforms.emplace_back(Transform());

	// Attach one of the Transform Components to each Entity in the Scene
	player1->attachComponent(&transforms[0], "transform");
	player2->attachComponent(&transforms[1], "transform");
	player3->attachComponent(&transforms[2], "transform");
	player4->attachComponent(&transforms[3], "transform");
	cheese->attachComponent(&transforms[4], "transform");
	sausage->attachComponent(&transforms[5], "transform");
	tomato->attachComponent(&transforms[6], "transform");
	dough->attachComponent(&transforms[7], "transform");
	countertop->attachComponent(&transforms[8], "transform");

	// Initialize transform components
	Transform* player1Transform = player1->getTransform();
	player1Transform->position = glm::vec3(-10, 2.3f, 3);
	player1Transform->rotation = glm::vec3(0, 0, 0);
	player1Transform->scale = glm::vec3(1, 1, 1);

	Transform* player2Transform = player2->getTransform();
	player2Transform->position = glm::vec3(100, 2.3f, 0);
	player2Transform->rotation = glm::vec3(0, 0, 0);
	player2Transform->scale = glm::vec3(1, 1, 1);

	Transform* player3Transform = player3->getTransform();
	player3Transform->position = glm::vec3(-100, 2.3f, 0);
	player3Transform->rotation = glm::vec3(0, 0, 0);
	player3Transform->scale = glm::vec3(1, 1, 1);

	Transform* player4Transform = player4->getTransform();
	player4Transform->position = glm::vec3(0, 2.3f, -100);
	player4Transform->rotation = glm::vec3(0, 0, 0);
	player4Transform->scale = glm::vec3(1, 1, 1);

	Transform* cheeseTransform = cheese->getTransform();
	cheeseTransform->position = glm::vec3(20, 5, 20);
	cheeseTransform->rotation = glm::vec3(0, 0, 0);
	cheeseTransform->scale = glm::vec3(30, 30, 30);

	Transform* sausageTransform = sausage->getTransform();
	sausageTransform->position = glm::vec3(-20, 5, 20);
	sausageTransform->rotation = glm::vec3(0, 0, 0);
	sausageTransform->scale = glm::vec3(30, 30, 30);

	Transform* tomatoTransform = tomato->getTransform();
	tomatoTransform->position = glm::vec3(20, 5, -20);
	tomatoTransform->rotation = glm::vec3(0, 0, 0);
	tomatoTransform->scale = glm::vec3(30, 30, 30);

	Transform* doughTransform = dough->getTransform();
	doughTransform->position = glm::vec3(-20, 5, -20);
	doughTransform->rotation = glm::vec3(0, 0, 0);
	doughTransform->scale = glm::vec3(30, 30, 30);

	Transform* counterTransform = countertop->getTransform();
	counterTransform->position = glm::vec3(0, 0, 0);
	counterTransform->rotation = glm::vec3(0, 0, 0);
	counterTransform->scale = glm::vec3(1, 1, 1);
	//-----------------------------------------------------------------------------------

	// Get a reference to the countertop's AudioSource to play background music
	// Leaving this here for now so people can turn the music on/off easily
	AudioSource* countertopAudioSource = countertop->getAudioSource();
	countertopAudioSource->gain = 0.01f; // Volume control
	countertopAudioSource->play("bg.wav"); // Comment this out to turn off the music on load

	// Track time
	float oldTime = glfwGetTime(), newTime = 0, deltaTime = 0;

	// Set movement control callbacks
	auto movementCallbacks = std::make_shared<MovementCallbacks>(&physics); 
	window.setCallbacks(movementCallbacks);

	// Set up controller inputs
	XboxController controllers = XboxController(&physics);

	//-----------------------------------------------------------------------------------
	// GameLogic stuff - will go in GameLogic eventually
	//-----------------------------------------------------------------------------------
	Inventory p1Inv, p2Inv, p3Inv, p4Inv;
	player1->attachComponent(&p1Inv, "inventory");
	player2->attachComponent(&p2Inv, "inventory");
	player3->attachComponent(&p3Inv, "inventory");
	player4->attachComponent(&p4Inv, "inventory");

	// GAME LOOP
	while (!window.shouldClose())
	{
		// INPUT
		glfwPollEvents();

		// READ CONTROLLERS
		controllers.checkControllers(); // sets analog/digital
		controllers.setButtonStateFromController(0); // Getting the input from player 1 controller
		//controllers.setButtonStateFromController(1); // Getting the input from player 1 controller
		//controllers.setButtonStateFromController(2); // Getting the input from player 1 controller
		//controllers.setButtonStateFromController(3); // Getting the input from player 1 controller

		// SIMULATE
		newTime = glfwGetTime();
		deltaTime = newTime - oldTime;
		g_systems.physics->update(deltaTime);
		oldTime = newTime;

		// RENDER
		window.clear();
		renderer.update();
		ui.update();
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