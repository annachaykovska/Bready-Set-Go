#pragma once
#include "../Inventory.h"
#include "../Physics/PhysicsSystem.h"
#include "../Navigation/IngredientTracker.h"
#include "GameLoopMode.h"


struct PlayerBase {
	float minX = 0;
	float maxX = 0;
	float minY = 0;
	float maxY = 0;
	float minZ = 0;
	float maxZ = 0;

	PlayerBase(float newMinX, float newMaxX, float newMinY, float newMaxY, float newMinZ, float newMaxZ) :
		minX(newMinX),
		maxX(newMaxX),
		minY(newMinY),
		maxY(newMaxY),
		minZ(newMinZ),
		maxZ(newMaxZ)
	{}
};


class GameLoopManager {
public:
	GameLoopManager();
	void resetGameLoopValues();
	void updateGameStageFromMenu(int numPlayers);
	void updateGameStageFromPause();
	void setEndStage();
	void gameActorsReset(PhysicsSystem* physics, IngredientTracker* ingredientTracker, Inventory* p1Inv, Inventory* p2Inv, Inventory* p3Inv, Inventory* p4Inv);
	int checkForWin();
	bool checkForPlayerInBase(Entity* player, PlayerBase base);

	// Constants
	const float mainMenuTimeoutLength;
	const float returnToMainMenuTimeoutLength;
	const float showPauseMenuTimeoutLength;

	// State of the game
	int gameStage;
	int winnerId;

	// Main menu things
	int menuSelectionNumber;
	bool isMenuItemSelected;
	bool isBackToMenuSelected;

	// Mid game things
	float countdownStart;
	bool isPaused;
	bool showPauseMenu;
	int pauseMenuSelection;
	bool isPauseMenuItemSelected;

	// End game things
	bool isGameExitSelected;
	bool isGameEnded;
	int endScreenGenerated;

	// To give a few seconds timeout
	float mainMenuTimeoutStart; // after starting game in main
	float returnToMainMenuTimeoutStart; // after returning to main
	float showPauseMenuTimeoutStart; // pressing the pause button

private:

};
