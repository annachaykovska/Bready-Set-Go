#include "GameLoopManager.h"
#include <glfw/glfw3.h>
#include <stdlib.h>
#include <time.h>

using namespace std;

extern Scene g_scene;

// Clockwise, starting from the bottom left if facing in from the center of the main counter (BL, TL, TR, BR)
//vector<PxVec3> player1Base = { PxVec3(38, 3.2, 50.2), PxVec3(38, 3.2, 65.2), PxVec3(25, 3.2, 65.2), PxVec3(38, 3.2, 50.2) };
//vector<PxVec3> player2Base = { PxVec3(20, 3.2, 50.2), PxVec3(20, 3.2, 65.2), PxVec3(3, 3.2, 65.2), PxVec3(3, 3.2, 50.2) };
//vector<PxVec3> player3Base = { PxVec3(-0.6, 3.2, 50.2), PxVec3(-0.6, 3.2, 65.2), PxVec3(-20, 3.2, 65.2), PxVec3(-20, 3.2, 50.2) };
//vector<PxVec3> player4Base = { PxVec3(-38, 3.2, 50.2), PxVec3(-38, 3.2, 65.2), PxVec3(-26, 3.2, 65.2), PxVec3(-26, 3.2, 50.2) };

PlayerBase player1Base = PlayerBase(25, 38, 0, 5, 50, 70);
PlayerBase player2Base = PlayerBase(3, 20, 0, 5, 50, 70);
PlayerBase player3Base = PlayerBase(-20, 0, 0, 5, 50, 70);
PlayerBase player4Base = PlayerBase(-38, -25, 0, 5, 50, 70);


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
, winnerId(0)
, countdownStart(-1)
, countdownStage(0)
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
	winnerId = 0;
	countdownStart = -1;
	countdownStage = 0;

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
			gameStage = GameLoopMode::START_COUNTDOWN;
			isMenuItemSelected = false;
			mainMenuTimeoutStart = glfwGetTime();
			g_scene.numPlayers = 1;
			countdownStart = glfwGetTime();
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
			gameStage = GameLoopMode::START_COUNTDOWN;
			isMenuItemSelected = false;
			mainMenuTimeoutStart = glfwGetTime();
			g_scene.numPlayers = numPlayers;
			countdownStart = glfwGetTime();
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

int GameLoopManager::checkForWin()
{
	Entity* player1 = g_scene.getEntity("player1");
	Inventory* p1Inv = (Inventory*)player1->getComponent("inventory");
	bool isPlayerInBase = checkForPlayerInBase(player1, player1Base);
	if (p1Inv->checkRecipeComplete(player1) && isPlayerInBase) {
		winnerId = 1;
	}

	Entity* player2 = g_scene.getEntity("player2");
	Inventory* p2Inv = (Inventory*)player2->getComponent("inventory");
	isPlayerInBase = checkForPlayerInBase(player2, player2Base);
	if (p2Inv->checkRecipeComplete(player2) && isPlayerInBase) {
		winnerId = 2;
	}


	Entity* player3 = g_scene.getEntity("player3");
	Inventory* p3Inv = (Inventory*)player3->getComponent("inventory");
	isPlayerInBase = checkForPlayerInBase(player3, player3Base);
	if (p3Inv->checkRecipeComplete(player3) && isPlayerInBase) {
		winnerId = 3;
	}

	Entity* player4 = g_scene.getEntity("player4");
	Inventory* p4Inv = (Inventory*)player4->getComponent("inventory");
	isPlayerInBase = checkForPlayerInBase(player4, player4Base);
	if (p4Inv->checkRecipeComplete(player4) && isPlayerInBase) {
		winnerId = 4;
	}

	return winnerId;
}

bool GameLoopManager::checkForPlayerInBase(Entity* player, PlayerBase base) {
	glm::vec3 currentLocation = player->getTransform()->position;
	if (currentLocation.x >= base.minX && currentLocation.x <= base.maxX &&
		currentLocation.y >= base.minY && currentLocation.y <= base.maxY &&
		currentLocation.z >= base.minZ && currentLocation.z <= base.maxZ) {
		return true;
	}

	return false;
}
