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
//#include "Rendering/DebugOverlay.h"
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
	// Players
	Entity* player1 = g_scene.getEntity("player1");
	Entity* player2 = g_scene.getEntity("player2");
	Entity* player3 = g_scene.getEntity("player3");
	Entity* player4 = g_scene.getEntity("player4");

	// Ingredients
	Entity* cheese = g_scene.getEntity("cheese");
	Entity* sausage = g_scene.getEntity("sausage");
	Entity* tomato = g_scene.getEntity("tomato");
	Entity* dough = g_scene.getEntity("dough");
	Entity* carrot = g_scene.getEntity("carrot");
	Entity* lettuce = g_scene.getEntity("lettuce");
	Entity* parsnip = g_scene.getEntity("parsnip");
	Entity* rice = g_scene.getEntity("rice");
	Entity* egg = g_scene.getEntity("egg");
	Entity* chicken = g_scene.getEntity("chicken");
	Entity* peas = g_scene.getEntity("peas");
	//Entity* soupbase = g_scene.getEntity("soupbase");
	//Entity* pumpkin = g_scene.getEntity("pumpkin");

	// World
	Entity* countertop = g_scene.getEntity("countertop");
	Entity* fan = g_scene.getEntity("fan");

	// Debug
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

	UISystem ui;
	g_systems.ui = &ui;

	g_scene.init(&physics);

	//-----------------------------------------------------------------------------------
	// INITIALIZE TRANSFORMS - will be handled by PhysicsSystem eventually
	//-----------------------------------------------------------------------------------
	// Create a container for Transform Components (will be handled by a system in the future)
	// and add some some new Transforms to it for the Entities
	std::vector<Transform> transforms;
	for (int i = 0; i < g_scene.entityCount(); i++)
		transforms.emplace_back(Transform());

	// Attach one of the Transform Components to each Entity in the Scene
	// Players
	player1->attachComponent(&transforms[0], "transform");
	player2->attachComponent(&transforms[1], "transform");
	player3->attachComponent(&transforms[2], "transform");
	player4->attachComponent(&transforms[3], "transform");

	// Ingredients
	cheese->attachComponent(&transforms[4], "transform");
	sausage->attachComponent(&transforms[5], "transform");
	tomato->attachComponent(&transforms[6], "transform");
	dough->attachComponent(&transforms[7], "transform");
	carrot->attachComponent(&transforms[8], "transform");
	lettuce->attachComponent(&transforms[9], "transform");
	parsnip->attachComponent(&transforms[10], "transform");
	rice->attachComponent(&transforms[11], "transform");
	egg->attachComponent(&transforms[12], "transform");
	chicken->attachComponent(&transforms[13], "transform");
	peas->attachComponent(&transforms[14], "transform");
	//soupbase->attachComponent(&transforms[15], "transform");
	//pumpkin->attachComponent(&transforms[16], "transform");

	// World
	countertop->attachComponent(&transforms[17], "transform");
	fan->attachComponent(&transforms[18], "transform");

	// Debug
	test->attachComponent(&transforms[19], "transform");

	// Initialize transform components
	Transform* counterTrans = countertop->getTransform();
	counterTrans->position.y = -78.0f;
	counterTrans->position.z = 50.0f;
	counterTrans->update();

	fan->getTransform()->position = glm::vec3(0.0f, 150.0f, -50.0f);
	fan->getTransform()->scale = glm::vec3(15.0f);
	fan->getTransform()->update();

	cheese->getTransform()->scale = glm::vec3(30, 30, 30);
	sausage->getTransform()->scale = glm::vec3(30, 30, 30);
	tomato->getTransform()->scale = glm::vec3(30, 30, 30);
	dough->getTransform()->scale = glm::vec3(30, 30, 30);

	carrot->getTransform()->scale = glm::vec3(30, 30, 30);
	carrot->getTransform()->position = glm::vec3(0, 5, 0);
	lettuce->getTransform()->scale = glm::vec3(30, 30, 30);
	lettuce->getTransform()->position = glm::vec3(-5, 5, 0);
	parsnip->getTransform()->scale = glm::vec3(30, 30, 30);
	parsnip->getTransform()->position = glm::vec3(-10, 5, 0);
	rice->getTransform()->scale = glm::vec3(30, 30, 30);
	rice->getTransform()->position = glm::vec3(-15, 5, 0);
	egg->getTransform()->scale = glm::vec3(30, 30, 30);
	egg->getTransform()->position = glm::vec3(-20, 5, 0);
	chicken->getTransform()->scale = glm::vec3(30, 30, 30);
	chicken->getTransform()->position = glm::vec3(-25, 5, 0);
	peas->getTransform()->scale = glm::vec3(30, 30, 30);
	peas->getTransform()->position = glm::vec3(-30, 5, 0);
	//soupbase->getTransform()->scale = glm::vec3(30, 30, 30);
	//soupbase->getTransform()->position = glm::vec3(-35, 5, 0);
	//pumpkin->getTransform()->scale = glm::vec3(30, 30, 30);
	//pumpkin->getTransform()->position = glm::vec3(-40, 5, 0);

	Transform* testTransform = test->getTransform();
	testTransform->position = glm::vec3(0, 3, 30);
	testTransform->scale = glm::vec3(1, 1, 1);
	//-----------------------------------------------------------------------------------

	// Get a reference to the countertop's AudioSource to play background music
	// Leaving this here for now so people can turn the music on/off easily
	AudioSource* countertopAudioSource = countertop->getAudioSource();
	audio.playMainMenuMusic(countertopAudioSource);
	
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

	// Set movement control callbacks
	auto movementCallbacks = std::make_shared<MovementCallbacks>(&physics); 
	window.setCallbacks(movementCallbacks);

	// Track Ingredient Locations
	IngredientTracker ingredientTracker(cheese->getTransform(), tomato->getTransform(), dough->getTransform(), sausage->getTransform());
	ui.initIngredientTracking(&ingredientTracker);

	// Set up game loop manager
	GameLoopManager gameLoop = GameLoopManager();

	// Set up controller inputs
	XboxController controllers = XboxController(&physics, &ui, &gameLoop);

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

	// Track time
	double t = 0.0;
	const double dt = 1.0/120.0;
	double currentTime = glfwGetTime();
	double accumulator = 0.0;

	// Change to 1 for submission
	gameLoop.gameStage = 1;

	// GAME LOOP
	while (!window.shouldClose() && !gameLoop.isGameExitSelected)
	{
		double newTime = glfwGetTime();
		double frameTime = newTime - currentTime;
		currentTime = newTime;

		accumulator += frameTime;

		// INPUT
		glfwPollEvents();
		glfwGetWindowSize(window.getWindow(), &g_systems.width, &g_systems.height);
		controllers.checkControllers(); // sets analog/digital

		while (accumulator >= dt)
		{
			// SIMULATE
			if (!gameLoop.isPaused) {
				g_systems.physics->update(dt, gameLoop.gameStage);
			}
			accumulator -= dt;
			t += dt;
		}

		// WINDOW
		window.clear();

		if (gameLoop.gameStage == 1) {
			controllers.setButtonStateFromControllerMainMenu(0); // Getting the input from player 1 controller
			//controllers.setButtonStateFromControllerMainMenu(1); // Getting the input from player 1 controller
			//controllers.setButtonStateFromControllerMainMenu(2); // Getting the input from player 1 controller
			//controllers.setButtonStateFromControllerMainMenu(3); // Getting the input from player 1 controller
			
			// RENDER
			ui.updateMainMenu(gameLoop.menuSelectionNumber);
			window.swapBuffer();
		}
		else if (gameLoop.gameStage == 2 || gameLoop.gameStage == 3) {
			//std::cout << navMesh.currentMeshSegment(player1->getTransform()->position)->id_ << std::endl;
			int winner = ui.checkForWin();
			if (winner != 0) {
				gameLoop.setEndStage();
			}
			controllers.setButtonStateFromControllerDriving(0, winner); // Getting the input from player 1 controller
			//controllers.setButtonStateFromControllerDriving(1); // Getting the input from player 1 controller
			//controllers.setButtonStateFromControllerDriving(2); // Getting the input from player 1 controller
			//controllers.setButtonStateFromControllerDriving(3); // Getting the input from player 1 controller
			
			// RENDER
			renderer.update();

			if (!g_systems.renderDebug)
			{
				ui.updateMiniMap(*player1->getTransform(), *player2->getTransform(), *player3->getTransform(), *player4->getTransform());
				ui.updateGame(gameLoop.endScreenGenerated);
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

		if (ui.checkForWin() != 0) {
			gameLoop.isGameEnded = true;
		}

		// UPDATE GAME STAGE
		if (gameLoop.isMenuItemSelected) {
			gameLoop.updateGameStageFromMenu();
			audio.stopMusic(countertopAudioSource);
			audio.playGameMusic(countertopAudioSource);
			gameLoop.isPaused = false;
		}

		// RESET game if end of game and menu selected
		else if (gameLoop.isBackToMenuSelected) {
			gameLoop.resetGameLoopValues();
			gameLoop.gameActorsReset(&physics, &p1Inv, &p2Inv, &p3Inv, &p4Inv);
			ingredientTracker = IngredientTracker(cheese->getTransform(), tomato->getTransform(), dough->getTransform(), sausage->getTransform());
			ui.initIngredientTracking(&ingredientTracker);

			audio.stopMusic(countertopAudioSource);
			audio.playMainMenuMusic(countertopAudioSource);
			gameLoop.isPaused = true;
		}
	}

	// Collect garbage
	physics.cleanupPhysics();
	profiler.cleanup();
	renderer.getShader().cleanup();
	glfwTerminate();

	return 0;
}
