#include <iostream>
#include <vector>
#include <Windows.h>
#include <WinUser.h>

#include "Window.h"
#include "Profiler.h"
#include "SystemManager.h"
#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Transform.h"
#include "Rendering/DebugOverlay.h"
#include "Rendering/RenderingSystem.h"
#include "Rendering/UISystem.h"
#include "Audio/AudioSystem.h"
#include "Navigation/NavMesh.h"
#include "Navigation/NavigationSystem.h"
#include "Physics/VehicleController.h"
#include "Inventory.h"
#include "DebugSettings.h"
#include "Navigation/IngredientTracker.h"
#include "Navigation/AIBrain.h"

// Global Scene holds all the Entities for easy reference
Scene g_scene;

// Global SystemManager holds all the Systems for easy reference
SystemManager g_systems;

int main()
{
	// Initialize GLFW library
	glfwInit();

	// Set window size
	g_systems.width = GetSystemMetrics(SM_CXSCREEN);
	g_systems.height = GetSystemMetrics(SM_CYSCREEN);

	bool fullScreen = false;

	// Create viewport window
	Window window(g_systems.width, g_systems.height, "Bready Set Go!!!", fullScreen);
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
	Entity* test = g_scene.getEntity("test");

	//-----------------------------------------------------------------------------------
	// INITIALIZE SYSTEMS
	//-----------------------------------------------------------------------------------
	PhysicsSystem physics;
	g_systems.physics = &physics;

	//DebugOverlay debugOverlay;
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
	for (int i = 0; i < 10; i++)
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
	test->attachComponent(&transforms[9], "transform");

	// Initialize transform components
	Transform* counterTrans = countertop->getTransform();
	counterTrans->position.y = -78.0f;
	counterTrans->position.z = 50.0f;
	counterTrans->update();

	Transform* cheeseTransform = cheese->getTransform();
	cheeseTransform->scale = glm::vec3(30, 30, 30);

	Transform* sausageTransform = sausage->getTransform();
	sausageTransform->scale = glm::vec3(30, 30, 30);

	Transform* tomatoTransform = tomato->getTransform();
	tomatoTransform->scale = glm::vec3(30, 30, 30);

	Transform* doughTransform = dough->getTransform();
	doughTransform->scale = glm::vec3(30, 30, 30);

	Transform* testTransform = test->getTransform();
	testTransform->position = glm::vec3(0, 3, 30);
	testTransform->scale = glm::vec3(1, 1, 1);
	//-----------------------------------------------------------------------------------

	// Get a reference to the countertop's AudioSource to play background music
	// Leaving this here for now so people can turn the music on/off easily
	AudioSource* countertopAudioSource = countertop->getAudioSource();
	countertopAudioSource->gain = 0.01f; // Volume control
	countertopAudioSource->loop = true;
	countertopAudioSource->play("bg.wav"); // Comment this out to turn off the music on load
	
	AudioSource* p2Audio = player2->getAudioSource();
	AudioSource* p3Audio = player3->getAudioSource();
	AudioSource* p4Audio = player4->getAudioSource();
	p2Audio->gain = 1.0f;
	//p2Audio->loop = true;
	//p2Audio->play("idle.wav");
	p3Audio->gain = 1.0f;
	//p3Audio->loop = true;
	//p3Audio->play("idle.wav");
	p4Audio->gain = 1.0f;
	//p4Audio->loop = true;
	//p4Audio->play("idle.wav");

	// Track time
	float oldTime = glfwGetTime(), newTime = 0, deltaTime = 0, accumulator = 0;
	float frameBeginTime = 0, frameEndTime = 0; 
	float fixedTimeInterval = 0, shortTimeInterval = 1.0f / 120.0f, longTimeInterval = 1.0f / 60.0f;

	// Set movement control callbacks
	auto movementCallbacks = std::make_shared<MovementCallbacks>(&physics); 
	window.setCallbacks(movementCallbacks);

	// Track Ingredient Locations
	IngredientTracker ingredientTracker(cheeseTransform, tomatoTransform, doughTransform, sausageTransform);
	ui.initIngredientTracking(&ingredientTracker);

	// Set up controller inputs
	XboxController controllers = XboxController(&physics, &ui);

	//-----------------------------------------------------------------------------------
	// GameLogic stuff - will go in GameLogic eventually
	//-----------------------------------------------------------------------------------
	NavMesh navMesh;
	Inventory p1Inv, p2Inv, p3Inv, p4Inv;

	NavigationSystem p2NavSystem(*player2, physics, navMesh, 2);
	AIBrain p2Brain(p2Inv, ingredientTracker, p2NavSystem);

	//debugOverlay.addDebugMesh(navMesh.getWireframe(), DEBUG_NAV_MESH);
	player1->attachComponent(&p1Inv, "inventory");
	player2->attachComponent(&p2Inv, "inventory");
	player2->attachComponent(&p2NavSystem, "navigation");
	player3->attachComponent(&p3Inv, "inventory");
	player4->attachComponent(&p4Inv, "inventory");

	// 1 = main menu, 2 = play
	int gameStage = 1;
	bool gameExit = false;

	// GAME LOOP
	while (!window.shouldClose() && !gameExit)
	{
		frameBeginTime = glfwGetTime();

		// INPUT
		glfwPollEvents();
		glfwGetWindowSize(window.getWindow(), &g_systems.width, &g_systems.height);
		controllers.checkControllers(); // sets analog/digital

		// SIMULATE
		g_systems.physics->update(deltaTime);

		// WINDOW
		window.clear();

		if (gameStage == 1) {
			controllers.setButtonStateFromControllerMainMenu(0); // Getting the input from player 1 controller
			//controllers.setButtonStateFromControllerMainMenu(1); // Getting the input from player 1 controller
			//controllers.setButtonStateFromControllerMainMenu(2); // Getting the input from player 1 controller
			//controllers.setButtonStateFromControllerMainMenu(3); // Getting the input from player 1 controller
			
			// RENDER
			ui.updateMainMenu(controllers.menuSelection);
			window.swapBuffer();
		}
		else if (gameStage == 2) {
			controllers.setButtonStateFromControllerDriving(0); // Getting the input from player 1 controller
			//controllers.setButtonStateFromControllerDriving(1); // Getting the input from player 1 controller
			//controllers.setButtonStateFromControllerDriving(2); // Getting the input from player 1 controller
			//controllers.setButtonStateFromControllerDriving(3); // Getting the input from player 1 controller
			
			// RENDER
			renderer.update();

			if (!g_systems.renderDebug)
			{
				ui.updateMiniMap(*player1->getTransform(), *player2->getTransform(), *player3->getTransform(), *player4->getTransform());
				ui.updateGame();
			}

			// Update the ImGUI profiler
			profiler.newFrame();
			profiler.update();

			window.swapBuffer();

			// AI + Navigation
			p2Brain.update();
			ingredientTracker.update();

			// AUDIO
			audio.update();
		}

		// UPDATE TIME
		// If this frame took longer than 1/60 s to compute, treat it as two frames instead
		if (frameEndTime - frameBeginTime > shortTimeInterval)
			fixedTimeInterval = longTimeInterval;

		// If the last frame took less than 1/60 s to compute, delay
		do 
		{
			frameEndTime = glfwGetTime();
			deltaTime = frameEndTime - frameBeginTime;
		} while (deltaTime < fixedTimeInterval);

		fixedTimeInterval = shortTimeInterval;

		// Update game stage
		if (controllers.menuItemSelected) { 
			if (controllers.menuSelection == 1) {// start game selected
				gameStage = 2;
			}
			else if (controllers.menuSelection == 2) { // exit
				gameExit = true;
			}
		}
		
	}

	// Collect garbage
	physics.cleanupPhysics();
	profiler.cleanup();
	renderer.getShader().cleanup();
	glfwTerminate();

	return 0;
}