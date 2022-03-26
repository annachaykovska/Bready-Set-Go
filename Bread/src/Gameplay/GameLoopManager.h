#pragma once


class GameLoopManager {
public:
	GameLoopManager();
	void resetBackToStart();
	void updateGameStageFromMenu();

	// Constants
	const float mainMenuTimeoutLength;
	const float returnTimeoutLength;

	// 1 = main menu, 2 = play
	int gameStage;

	int menuSelectionNumber;
	bool isMenuItemSelected;
	bool isBackToMenuSelected;

	bool isGameExitSelected;
	bool isGameEnded;

	// To give a few seconds timeout
	float mainMenuTimeoutStart; // after starting game in main
	float returnTimeoutStart; // after returning to main

private:

};
