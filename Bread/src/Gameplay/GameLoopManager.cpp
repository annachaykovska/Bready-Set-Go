#include "GameLoopManager.h"
#include <glfw/glfw3.h>
#include <stdlib.h>
#include <time.h>

GameLoopManager::GameLoopManager() : gameStage(1) // set this to 2 to skip the menu
, menuSelectionNumber(1)
, isMenuItemSelected(false)
, isBackToMenuSelected(false)
, isGameExitSelected(false)
, isGameEnded(false)
, mainMenuTimeoutStart(-1)
, returnToMainMenuTimeoutStart(-1)
, mainMenuTimeoutLength(3)
, returnToMainMenuTimeoutLength(3)
, endScreenGenerated(-1)
, isPaused(false)
{}


void GameLoopManager::resetGameLoopValues() {
	gameStage = 1;
	isBackToMenuSelected = false;
	isGameEnded = false;
	returnToMainMenuTimeoutStart = glfwGetTime();
	endScreenGenerated = -1;
	return;
}

void GameLoopManager::updateGameStageFromMenu() {
	if (menuSelectionNumber == 1) { // start game selected
		gameStage = 2;
		isMenuItemSelected = false;
		mainMenuTimeoutStart = glfwGetTime();
	}
	else if (menuSelectionNumber == 2) { // exit
		isGameExitSelected = true;
	}
	return;
}

void GameLoopManager::setEndStage() {
	gameStage = 3;
	if (endScreenGenerated == -1) {
		// Add some random generation to final message 
		srand((unsigned)time(0));
		endScreenGenerated = rand() % 4;
	}
}

void GameLoopManager::gameActorsReset(PhysicsSystem* physics, Inventory* p1Inv, Inventory* p2Inv, Inventory* p3Inv, Inventory* p4Inv) {
	// Reset player positions
	physics->respawnPlayer(1);
	physics->respawnPlayer(2);
	physics->respawnPlayer(3);
	physics->respawnPlayer(4);

	// Reset all ingredient positions 
	physics->randomizeIngredientLocations();

	// Reset player inventory
	p1Inv->clearAllIngredients();
	p2Inv->clearAllIngredients();
	p3Inv->clearAllIngredients();
	p4Inv->clearAllIngredients();
}