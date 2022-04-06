#include "GameLoopManager.h"
#include <glfw/glfw3.h>
#include <stdlib.h>
#include <time.h>


extern Scene g_scene;


GameLoopManager::GameLoopManager() : gameStage(GameLoopMode::MENU_START) 
, menuSelectionNumber(1)
, isMenuItemSelected(false)
, isBackToMenuSelected(false)
, isGameExitSelected(false)
, isGameEnded(false)
, mainMenuTimeoutStart(-1)
, returnToMainMenuTimeoutStart(-1)
, mainMenuTimeoutLength(0.5)
, returnToMainMenuTimeoutLength(0.5)
, endScreenGenerated(-1)
, isPaused(false)
, showPauseMenu(false)
, pauseMenuSelection(1)
, showPauseMenuTimeoutLength(0.5)
, showPauseMenuTimeoutStart(-1)
, isPauseMenuItemSelected(false)
{}


void GameLoopManager::resetGameLoopValues() {
	gameStage = GameLoopMode::MENU_START;
	isBackToMenuSelected = false;
	isGameEnded = false;
	returnToMainMenuTimeoutStart = glfwGetTime();
	endScreenGenerated = -1;
	pauseMenuSelection = 1;
	showPauseMenu = false;
	isPauseMenuItemSelected = false;

	g_scene.numPlayers = 1;
	return;
}

void GameLoopManager::updateGameStageFromMenu(int numPlayers) {
	if (gameStage == GameLoopMode::MENU_START) {
		if (menuSelectionNumber == 1) { // start game selected
			gameStage = GameLoopMode::MENU_SINGLE_MULTI_SELECTION;
			isMenuItemSelected = false;
			mainMenuTimeoutStart = glfwGetTime();
		}
		else if (menuSelectionNumber == 2) { // exit
			isGameExitSelected = true;
		}
	}
	else if (gameStage == GameLoopMode::MENU_SINGLE_MULTI_SELECTION) {
		if (menuSelectionNumber == 1) { // start game selected
			gameStage = GameLoopMode::MAIN_GAME_PLAY;
			isMenuItemSelected = false;
			mainMenuTimeoutStart = glfwGetTime();
			g_scene.numPlayers = 1;
		}
		else if (menuSelectionNumber == 2) { // multiplayer
			gameStage = GameLoopMode::MENU_MULTI_CONNECT;
			isMenuItemSelected = false;
			mainMenuTimeoutStart = glfwGetTime();
			menuSelectionNumber = 1;
		}
	}
	else if (gameStage == GameLoopMode::MENU_MULTI_CONNECT) {
		if (menuSelectionNumber == 1) { // start game selected
			gameStage = GameLoopMode::MAIN_GAME_PLAY;
			isMenuItemSelected = false;
			mainMenuTimeoutStart = glfwGetTime();
			g_scene.numPlayers = numPlayers;
		}
	}
	return;
}

void GameLoopManager::updateGameStageFromPause() {
	if (pauseMenuSelection == 1) { // continue
		isPauseMenuItemSelected = false;
		showPauseMenu = false;
		showPauseMenuTimeoutStart = glfwGetTime();
		isPaused = false;
	}
	else if (pauseMenuSelection == 2) { // exit
		gameStage = GameLoopMode::MENU_START;
		isBackToMenuSelected = true;
		isPauseMenuItemSelected = false;
		showPauseMenuTimeoutStart = glfwGetTime();
	}
	return;
}


void GameLoopManager::setEndStage() {
	gameStage = GameLoopMode::END_GAME;
	if (endScreenGenerated == -1) {
		// Add some random generation to final message 
		srand((unsigned)time(0));
		endScreenGenerated = rand() % 4;
	}
}

void GameLoopManager::gameActorsReset(PhysicsSystem* physics, IngredientTracker* ingredientTracker, Inventory* p1Inv, Inventory* p2Inv, Inventory* p3Inv, Inventory* p4Inv) {
	// Reset player positions
	physics->respawnPlayer(1);
	physics->respawnPlayer(2);
	physics->respawnPlayer(3);
	physics->respawnPlayer(4);

	// Reset all ingredient positions 
	physics->randomizeIngredientLocations();
	physics->updateFoodTransforms(true);
	ingredientTracker->updateCheeseTransformSource(g_scene.getEntity("cheese")->getTransform());
	ingredientTracker->updateDoughTransformSource(g_scene.getEntity("dough")->getTransform());
	ingredientTracker->updateSausageTransformSource(g_scene.getEntity("sausage")->getTransform());
	ingredientTracker->updateTomatoTransformSource(g_scene.getEntity("tomato")->getTransform());
	ingredientTracker->updateCarrotTransformSource(g_scene.getEntity("carrot")->getTransform());
	ingredientTracker->updateLettuceTransformSource(g_scene.getEntity("lettuce")->getTransform());
	ingredientTracker->updateParsnipTransformSource(g_scene.getEntity("parsnip")->getTransform());
	ingredientTracker->updateRiceTransformSource(g_scene.getEntity("rice")->getTransform());
	ingredientTracker->updateEggTransformSource(g_scene.getEntity("egg")->getTransform());
	ingredientTracker->updateChickenTransformSource(g_scene.getEntity("chicken")->getTransform());
	ingredientTracker->updatePeasTransformSource(g_scene.getEntity("peas")->getTransform());

	// Reset player inventory
	p1Inv->clearAllIngredients();
	p2Inv->clearAllIngredients();
	p3Inv->clearAllIngredients();
	p4Inv->clearAllIngredients();
}