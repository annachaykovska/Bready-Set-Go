#pragma once


class GameLoopManager {
public:
	GameLoopManager();
	void resetBackToStart();
	void updateGameStageFromMenu();
	void setEndStage();

	// Constants
	const float mainMenuTimeoutLength;
	const float returnTimeoutLength;

	// State of the game
	// 1 = main menu, 2 = play, 3 = game ended (no collision updates from here)
	int gameStage;

	// Main menu things
	int menuSelectionNumber;
	bool isMenuItemSelected;
	bool isBackToMenuSelected;

	// End game things
	bool isGameExitSelected;
	bool isGameEnded;

	// To give a few seconds timeout
	float mainMenuTimeoutStart; // after starting game in main
	float returnTimeoutStart; // after returning to main

private:

};
