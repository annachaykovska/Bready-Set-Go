#pragma once
#include "../Inventory.h"
#include "../Physics/PhysicsSystem.h"
#include "../Navigation/IngredientTracker.h"


class GameLoopManager {
public:
	GameLoopManager();
	void resetGameLoopValues();
	void updateGameStageFromMenu();
	void updateGameStageFromPause();
	void setEndStage();
	void gameActorsReset(PhysicsSystem* physics, IngredientTracker* ingredientTracker, Inventory* p1Inv, Inventory* p2Inv, Inventory* p3Inv, Inventory* p4Inv);

	// Constants
	const float mainMenuTimeoutLength;
	const float returnToMainMenuTimeoutLength;
	const float showPauseMenuTimeoutLength;

	// State of the game
	// 1 = main menu, 2 = play, 3 = game ended (no collision updates from here)
	int gameStage;

	// Main menu things
	int menuSelectionNumber;
	bool isMenuItemSelected;
	bool isBackToMenuSelected;

	// Mid game things
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
