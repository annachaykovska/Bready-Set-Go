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
#include "Gameplay/Recipe.h"
#include "Gameplay/GameLoopManager.h"
#include "Timer.h"

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
	g_systems.profiler = &profiler;

	// Change this at runtime in the main menu
	g_scene.numPlayers = 1;

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
	g_systems.physics->initialize(); // Needs to happen after renderer is constructed

	AudioSystem audio;
	g_systems.audio = &audio;

	UISystem ui;
	g_systems.ui = &ui;

	//-----------------------------------------------------------------------------------
	// INITIALIZE TRANSFORMS
	//-----------------------------------------------------------------------------------
	// TODO Move out of main
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
	fan->getTransform()->position = glm::vec3(0.0f, 150.0f, -50.0f);
	fan->getTransform()->scale = glm::vec3(15.0f);
	fan->getTransform()->update();

	Transform* testTransform = test->getTransform();
	testTransform->position = glm::vec3(0, 3, 30);
	testTransform->scale = glm::vec3(0.01f, 0.01f, 0.01f);
	//-----------------------------------------------------------------------------------

	// Get a reference to the countertop's AudioSource to play background music
	// Leaving this here for now so people can turn the music on/off easily
	AudioSource* countertopAudioSource = countertop->getAudioSource();
	countertopAudioSource->gain = 0.05f;
	audio.playMainMenuMusic(countertopAudioSource);
	
	AudioSource* p2Audio = player2->getAudioSource();
	AudioSource* p3Audio = player3->getAudioSource();
	AudioSource* p4Audio = player4->getAudioSource();
	p2Audio->gain = 1.0f;
	p3Audio->gain = 1.0f;
	p4Audio->gain = 1.0f;

	// Set movement control callbacks
	auto movementCallbacks = std::make_shared<MovementCallbacks>(&physics); 
	window.setCallbacks(movementCallbacks);

	// Track Ingredient Locations
	IngredientTracker ingredientTracker;
	g_systems.tracker = &ingredientTracker;
	ui.initIngredientTracking(&ingredientTracker);

	// Set up game loop manager
	GameLoopManager gameLoop = GameLoopManager();
	g_systems.loop = &gameLoop;

	// Set up controller inputs
	XboxController controllers = XboxController(&physics, &ui, &gameLoop);

	//-----------------------------------------------------------------------------------
	// GameLogic stuff - will go in GameLogic eventually
	//-----------------------------------------------------------------------------------
	Recipe pizza(Ingredient::Cheese, Ingredient::Dough, Ingredient::Sausage, Ingredient::Tomato, "Pizza");
	Recipe wrap(Ingredient::Rice, Ingredient::Chicken, Ingredient::Lettuce, Ingredient::Dough, "Wrap");
	Recipe salad(Ingredient::Lettuce, Ingredient::Tomato, Ingredient::Parsnip, Ingredient::Carrot, "Salad");
	Recipe omelette(Ingredient::Egg, Ingredient::Lettuce, Ingredient::Cheese, Ingredient::Peas, "Omelette");

	NavMesh navMesh;
	Inventory p1Inv, p2Inv, p3Inv, p4Inv;

	NavigationSystem p2NavSystem(*player2, physics, navMesh, 2);
	glm::vec3 p2Start = glm::vec3(11, 3, 60);
	AIBrain p2Brain(p2Inv, ingredientTracker, p2NavSystem, p2Start);

	NavigationSystem p3NavSystem(*player3, physics, navMesh, 3);
	glm::vec3 p3Start = glm::vec3(-31, 3, 60);
	AIBrain p3Brain(p3Inv, ingredientTracker, p3NavSystem, p3Start);

	NavigationSystem p4NavSystem(*player4, physics, navMesh, 4);
	glm::vec3 p4Start = glm::vec3(-11.5, 3, 60);
	AIBrain p4Brain(p4Inv, ingredientTracker, p4NavSystem, p4Start);
	
	player1->attachComponent(&p1Inv, "inventory");
	player1->attachComponent(&pizza, "recipe");

	player2->attachComponent(&p2Inv, "inventory");
	player2->attachComponent(&p2NavSystem, "navigation");
	player2->attachComponent(&omelette, "recipe");
	p2Brain.setRecipe(&omelette);

	player3->attachComponent(&p3Inv, "inventory");
	player3->attachComponent(&wrap, "recipe");
	p3Brain.setRecipe(&wrap);

	player4->attachComponent(&p4Inv, "inventory");
	player4->attachComponent(&salad, "recipe");
	p4Brain.setRecipe(&salad);

	// Track time
	double t = 0.0;
	const double dt = 1.0/120.0;
	double currentTime = glfwGetTime();
	double accumulator = 0.0;

	gameLoop.gameStage = GameLoopMode::MENU_START;
	//gameLoop.gameStage = GameLoopMode::MAIN_GAME_PLAY; // Skips menus

	// Call it a single time
	g_systems.physics->update(dt, gameLoop.gameStage);

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
			if (gameLoop.gameStage == GameLoopMode::MAIN_GAME_PLAY || gameLoop.gameStage == GameLoopMode::END_GAME) {
				if (!gameLoop.isPaused || !gameLoop.showPauseMenu) {
					g_systems.physics->update(dt, gameLoop.gameStage);
				}
			}

			accumulator -= dt;
			t += dt;
		}

		// WINDOW
		window.clear();

		if (gameLoop.gameStage == GameLoopMode::MENU_START || gameLoop.gameStage == GameLoopMode::MENU_SINGLE_MULTI_SELECTION || gameLoop.gameStage == GameLoopMode::MENU_MULTI_CONNECT) {
			controllers.setButtonStateFromControllerMainMenu(0, (AudioSource*)player1->getComponent("menuAudio")); // Getting the input from player 1 controller
			controllers.setButtonStateFromControllerMainMenu(1, (AudioSource*)player1->getComponent("menuAudio")); // Getting the input from player 2 controller
			controllers.setButtonStateFromControllerMainMenu(2, (AudioSource*)player1->getComponent("menuAudio")); // Getting the input from player 3 controller
			controllers.setButtonStateFromControllerMainMenu(3, (AudioSource*)player1->getComponent("menuAudio")); // Getting the input from player 4 controller
			
			// RENDER
			ui.updateMainMenu(gameLoop.menuSelectionNumber, gameLoop.gameStage, controllers.getNumberConnectedControllers());
			window.swapBuffer();
		}
		else if (gameLoop.gameStage == GameLoopMode::START_COUNTDOWN || gameLoop.gameStage == GameLoopMode::MAIN_GAME_PLAY || gameLoop.gameStage == GameLoopMode::END_GAME) {
			if (gameLoop.gameStage == GameLoopMode::START_COUNTDOWN) {
				// 3
				if (glfwGetTime() - gameLoop.countdownStart > 1 && gameLoop.countdownStage == 0) {
					countertopAudioSource->loop = false;
					countertopAudioSource->play("countdown_count.wav");
					gameLoop.countdownStage = 1;
				}
				// 2
				if (glfwGetTime() - gameLoop.countdownStart > 2 && gameLoop.countdownStage == 1) {
					countertopAudioSource->play("countdown_count.wav");
					gameLoop.countdownStage = 2;
				}
				// 1
				if (glfwGetTime() - gameLoop.countdownStart > 3 && gameLoop.countdownStage == 2) {
					countertopAudioSource->play("countdown_count.wav");
					gameLoop.countdownStage = 3;
				}
				// START
				if (glfwGetTime() - gameLoop.countdownStart > 4 && gameLoop.countdownStage == 3) {
					countertopAudioSource->play("countdown_go.wav");
					gameLoop.countdownStage = 4;
					gameLoop.gameStage = GameLoopMode::MAIN_GAME_PLAY;
				}
			}
			
			// TODO: Move out of main and make less dependent
			pizza.updateRecipeProgress(p1Inv);
			omelette.updateRecipeProgress(p2Inv);
			wrap.updateRecipeProgress(p3Inv);
			salad.updateRecipeProgress(p4Inv);

			//std::cout << navMesh.currentMeshSegment(player1->getTransform()->position)->id_ << std::endl;
			int winner = gameLoop.checkForWin();
			if (winner != 0) {
				gameLoop.setEndStage();
			}

			controllers.setButtonStateFromControllerDriving(0, winner, (AudioSource*)player1->getComponent("menuAudio")); // Getting the input from player 1 controller
			controllers.setButtonStateFromControllerDriving(1, winner, (AudioSource*)player1->getComponent("menuAudio")); // Getting the input from player 2 controller
			controllers.setButtonStateFromControllerDriving(2, winner, (AudioSource*)player1->getComponent("menuAudio")); // Getting the input from player 3 controller
			controllers.setButtonStateFromControllerDriving(3, winner, (AudioSource*)player1->getComponent("menuAudio")); // Getting the input from player 4 controller
			
			// RENDER
			renderer.update();

			// Update the ImGUI profiler
			profiler.newFrame();
			profiler.update();

			window.swapBuffer();

			// AI + Navigation
			ingredientTracker.update();
			if (!gameLoop.isGameEnded)
			{
				if (g_scene.numPlayers <= 3)
					p4Brain.update();
				if (g_scene.numPlayers <= 2)
					p3Brain.update();
				if (g_scene.numPlayers <= 1)
					p2Brain.update();
			}

			// AUDIO
			audio.update(dt, gameLoop.gameStage);
		}

		if (gameLoop.checkForWin() != 0) {
			if (!gameLoop.isGameEnded) {
				AudioSource* bgMusic = (AudioSource*)g_scene.getEntity("player1")->getComponent("bg");
				bgMusic->stop();
			}
			gameLoop.isGameEnded = true;
		}

		// UPDATE GAME STAGE
		if (gameLoop.isMenuItemSelected) {
			gameLoop.updateGameStageFromMenu(controllers.getNumberConnectedControllers());
			if (gameLoop.gameStage >= GameLoopMode::START_COUNTDOWN)
				audio.turnOffAllAudio();
			// The game audio is played by the update function, no need to play it here
			gameLoop.isPaused = false;
		}
		// PAUSE MENU
		else if (gameLoop.isPauseMenuItemSelected && !gameLoop.isBackToMenuSelected) {
			gameLoop.updateGameStageFromPause();
		}
		// RESET game if end of game or pause menu and menu selected
		else if (gameLoop.isBackToMenuSelected) {
			gameLoop.resetGameLoopValues();
			gameLoop.gameActorsReset(&physics, &ingredientTracker, &p1Inv, &p2Inv, &p3Inv, &p4Inv);

			player1->bannerSoundPlayed = false;
			player2->bannerSoundPlayed = false;
			player3->bannerSoundPlayed = false;
			player4->bannerSoundPlayed = false;
			audio.turnOffAllAudio();
			audio.playMainMenuMusic(countertopAudioSource);

			gameLoop.isPaused = true;
			g_systems.physics->update(dt, gameLoop.gameStage);
		}
	}

	// Collect garbage
	physics.cleanupPhysics();
	profiler.cleanup();
	renderer.getShader().cleanup();
	glfwTerminate();

	return 0;
}
