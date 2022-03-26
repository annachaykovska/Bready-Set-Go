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
, returnTimeoutStart(-1)
, mainMenuTimeoutLength(3)
, returnTimeoutLength(3)
, endScreenGenerated(-1)
{}


void GameLoopManager::resetBackToStart() {
	gameStage = 1;
	isBackToMenuSelected = false;
	isGameEnded = false;
	returnTimeoutStart = glfwGetTime();
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