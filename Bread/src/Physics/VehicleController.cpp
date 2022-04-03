/*
* Keyboard and Controller callback handler
*/
#include "VehicleController.h"

using namespace std;

XboxController::XboxController(PhysicsSystem* physicsSystem, UISystem* uiSystem, GameLoopManager* gameLoopManager) : forwards(true) {
	this->physics = physicsSystem;
	this->ui = uiSystem;
	this->gameLoop = gameLoopManager;
	this->y_held = false;
}

void XboxController::checkControllers() {
	int controllerId = -1;

	for (DWORD i = 0; i < XUSER_MAX_COUNT && controllerId == -1; i++)
	{
		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));

		if (XInputGetState(i, &state) == ERROR_SUCCESS) {
			controllerId = i;

		}
	}

	if (controllerId != -1)
	{
		physics->setAnalogInputs(true);
	}
	else {
		physics->setAnalogInputs(false);
	}
}

_XINPUT_STATE XboxController::getControllerState(int controllerId) {
	XINPUT_STATE state;
	ZeroMemory(&state, sizeof(XINPUT_STATE));

	DWORD dwResult;
	dwResult = XInputGetState(controllerId, &state);
	return state;
}

float XboxController::getDeadZone(float x, float y, float deadzone) {
	//determine how far the controller is pushed
	float magnitude = sqrt(x * x + y * y);

	//determine the direction the controller is pushed
	float normalizedLX = x / magnitude;
	float normalizedLY = y / magnitude;

	float normalizedMagnitude = 0;

	//check if the controller is outside a circular dead zone
	if (magnitude > deadzone)
	{
		//clip the magnitude at its expected maximum value
		if (magnitude > 32767) magnitude = 32767;

		//adjust magnitude relative to the end of the dead zone
		magnitude -= deadzone;

		//optionally normalize the magnitude with respect to its expected range
		//giving a magnitude value of 0.0 to 1.0
		normalizedMagnitude = magnitude / (32767 - deadzone);
	}
	else //if the controller is in the deadzone zero out the magnitude
	{
		magnitude = 0.0;
		normalizedMagnitude = 0.0;
	}

	return normalizedMagnitude;
}

void XboxController::setButtonStateFromControllerMainMenu(int controllerId) {
	physx::PxVehicleDrive4WRawInputData* input;
	if (controllerId == 1)
		input = &physics->mVehicleInputDataPlayer2;
	else if (controllerId == 2)
		input = &physics->mVehicleInputDataPlayer3;
	else if (controllerId == 3)
		input = &physics->mVehicleInputDataPlayer4;
	else
		input = &physics->mVehicleInputDataPlayer1; // Defaults to player 1
	XINPUT_STATE state = getControllerState(controllerId);

	// Left Thumb
	float thumbLeftX = state.Gamepad.sThumbLX;
	float thumbLeftY = state.Gamepad.sThumbLY;

	float thumbLeftDeadZone = getDeadZone(thumbLeftX, thumbLeftY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);

	// A button
	bool A_button_pressed = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0); // accept choice

	float newTime = glfwGetTime();
	if (newTime - gameLoop->returnToMainMenuTimeoutStart > gameLoop->returnToMainMenuTimeoutLength) {
		gameLoop->returnToMainMenuTimeoutStart = -1;
		if (A_button_pressed) {
			gameLoop->isMenuItemSelected = true; 
		}
	}

	if (thumbLeftY > 0.0 && thumbLeftDeadZone > 0.1) {
		if (gameLoop->menuSelectionNumber == 2) {
			gameLoop->menuSelectionNumber = 1;
		}
		
	}
	else if (thumbLeftY < 0.0 && thumbLeftDeadZone > 0.1) {
		if (gameLoop->menuSelectionNumber == 1) {
			gameLoop->menuSelectionNumber = 2;
		}
	}
}

void XboxController::setButtonStateFromControllerDriving(int controllerId, bool gameCompleted) {
	// Get the correct input data for the controller
	physx::PxVehicleDrive4WRawInputData* input;
	if (controllerId == 1)
		input = &physics->mVehicleInputDataPlayer2;
	else if (controllerId == 2)
		input = &physics->mVehicleInputDataPlayer3;
	else if (controllerId == 3)
		input = &physics->mVehicleInputDataPlayer4;
	else
		input = &physics->mVehicleInputDataPlayer1; // Defaults to player 1
	XINPUT_STATE state = getControllerState(controllerId);

	// Left Thumb
	float thumbLeftX = state.Gamepad.sThumbLX;
	float thumbLeftY = state.Gamepad.sThumbLY;

	float thumbLeftDeadZone = getDeadZone(thumbLeftX, thumbLeftY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);

	// Right Thumb
	float thumbRightX = state.Gamepad.sThumbRX;
	float thumbRightY = state.Gamepad.sThumbRY;

	float thumbRightDeadZone = getDeadZone(thumbRightX, thumbRightY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);

	// Left trigger
	float triggerLeft = state.Gamepad.bLeftTrigger;

	// Right trigger
	float triggerRight = state.Gamepad.bRightTrigger;

	// Other buttons
	bool X_button_pressed = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_X) != 0); // Hand brake
	bool A_button_pressed = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0); // Accept game over back to main menu
	bool Y_button_pressed = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) != 0); // Magnet ability
	bool START_button_pressed = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_START) != 0); // RESET
	bool BACK_button_pressed = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) != 0); // pause

	if (Y_button_pressed && !y_held) { // y just pressed
		//physics->magnet(1);
		physics->magnetCheckStealing(1, Y_button_pressed, !y_held);
		y_held = true;
	}
	else if (Y_button_pressed) { // y held
		//physics->magnet(1);
		physics->magnetCheckStealing(1, Y_button_pressed, !y_held);
	}
	else { // y not pressed
		physics->magnetCheckStealing(1, Y_button_pressed, !y_held);
		y_held = false;
	}

	if (X_button_pressed)
	{
		input->setAnalogHandbrake(1.0f);
	}
	else
	{
		input->setAnalogHandbrake(0.0f);
	}

	// Accept exit conditions back to the menu
	float currentTime = glfwGetTime();
	if (currentTime - gameLoop->mainMenuTimeoutStart > gameLoop->mainMenuTimeoutLength) {
		gameLoop->mainMenuTimeoutStart = -1;
		if (A_button_pressed && gameCompleted) {
			gameLoop->isBackToMenuSelected = true;
		}
	}

	// Pause menu
	if (currentTime - gameLoop->showPauseMenuTimeoutStart > gameLoop->showPauseMenuTimeoutLength) {
		gameLoop->showPauseMenuTimeoutStart = -1;
		if (BACK_button_pressed) {
			gameLoop->showPauseMenuTimeoutStart = currentTime;
			gameLoop->showPauseMenu = !gameLoop->showPauseMenu;
			if (gameLoop->showPauseMenu) {
				gameLoop->isPaused = true;
			}
		}
	}

	if (A_button_pressed && gameLoop->showPauseMenu) {
		gameLoop->isPauseMenuItemSelected = true;
	}

	// Respawn player
	if (START_button_pressed)
	{
		physics->respawnPlayer(controllerId + 1);
	}

	float analogVal;
	float analogVal2;
	if (thumbRightX <= 0) { // left
		physics->setViewDirectionalInfluence(thumbRightDeadZone);
	}
	if (thumbRightX > 0) { // right
		analogVal = -1.0 * thumbRightDeadZone;
		physics->setViewDirectionalInfluence(analogVal);
	}

	// TRIGGER PRESSED
	if (triggerLeft > 0.1 && triggerRight > 0.1) { // brake
		if (physics->mVehiclePlayer1->computeForwardSpeed()<0) {
			analogVal = triggerRight / 255.0f;
			analogVal2 = triggerLeft / 255.0f;
		}
		else {
			analogVal = triggerLeft / 255.0f;
			analogVal2 = triggerRight / 255.0f;
		}
		//analogVal = std::max(std::min(powf(analogVal, 1.0001), 1.0f),0.f);
		//std::cout << analogVal<<std::endl;
		//float step = analogVal * 2;
		//analogVal = -(1 / (10 * (step - 2.1))) - 0.048;
		input->setAnalogBrake(analogVal);
		input->setAnalogAccel(analogVal2);
	}
	else if (triggerRight > 0.1) { // Forward/Break when backwards
		if (physics->mVehiclePlayer1->mDriveDynData.mCurrentGear != snippetvehicle::PxVehicleGearsData::eFIRST)
			physics->mVehiclePlayer1->mDriveDynData.forceGearChange(snippetvehicle::PxVehicleGearsData::eFIRST);
		
		if (physics->mVehiclePlayer1->computeForwardSpeed() < -30)
		{
			physics->mVehiclePlayer1->mDriveDynData.setEngineRotationSpeed(0.f);
			input->setAnalogBrake(1.f);
		}
		else if (physics->mVehiclePlayer1->computeForwardSpeed() < 45)
		{
			analogVal = triggerRight / 255;
			input->setAnalogAccel(analogVal);
		}
		else
		{
			input->setAnalogAccel(0);
		}
		input->setAnalogBrake(0);
	}
	else if (triggerLeft > 0.1){ // Reverse/Break when forward

		if (physics->mVehiclePlayer1->mDriveDynData.mCurrentGear != snippetvehicle::PxVehicleGearsData::eREVERSE)
			physics->mVehiclePlayer1->mDriveDynData.forceGearChange(snippetvehicle::PxVehicleGearsData::eREVERSE);
		if (physics->mVehiclePlayer1->computeForwardSpeed() > 50)
		{
			physics->mVehiclePlayer1->mDriveDynData.setEngineRotationSpeed(0.f);
			input->setAnalogBrake(1.f);
		}
		else if (abs(physics->mVehiclePlayer1->computeForwardSpeed()) < 20)
		{
			analogVal = triggerLeft / 255;
			input->setAnalogAccel(analogVal);
		}
		else
		{
			input->setAnalogAccel(0);
		}
		input->setAnalogBrake(0);
	}
	else
	{
		physics->mVehiclePlayer1->mDriveDynData.forceGearChange(snippetvehicle::PxVehicleGearsData::eNEUTRAL);	
		if (abs(physics->mVehiclePlayer1->computeForwardSpeed()) < 1)
		{
			physics->mVehiclePlayer1->mWheelsDynData.setToRestState();
		}
	}

	if (gameLoop->showPauseMenu) {
		if (thumbLeftY > 0.0 && thumbLeftDeadZone > 0.1) {
			if (gameLoop->pauseMenuSelection == 2) {
				gameLoop->pauseMenuSelection = 1;
			}

		}
		else if (thumbLeftY < 0.0 && thumbLeftDeadZone > 0.1) {
			if (gameLoop->pauseMenuSelection == 1) {
				gameLoop->pauseMenuSelection = 2;
			}
		}
	}
	else {
		if (thumbLeftX <= 0 && thumbLeftDeadZone > 0.1) { // left
			float step = thumbLeftDeadZone * 2;
			if (step > 0.143)
			{
				// Using this trial-and-error found function to make steering more natural
				thumbLeftDeadZone = -(1 / (10 * (step - 2.1))) - 0.048;
			}
			else
			{
				thumbLeftDeadZone = 0;
			}
			if (thumbLeftDeadZone > 1)
			{
				thumbLeftDeadZone = 1;
			}
			input->setAnalogSteer(thumbLeftDeadZone);
			physics->setTurnDirectionalInfluence(thumbLeftDeadZone);

		}
		if (thumbLeftX > 0 && thumbLeftDeadZone > 0.1) { // right
			float step = thumbLeftDeadZone * 2;
			if (step > 0.143)
			{
				// Using this trial-and-error found function to make steering more natural
				thumbLeftDeadZone = -(1 / (10 * (step - 2.1))) - 0.048;
			}
			else
			{
				thumbLeftDeadZone = 0;
			}
			if (thumbLeftDeadZone > 1)
			{
				thumbLeftDeadZone = 1;
			}
			analogVal = -1.0 * thumbLeftDeadZone;
			input->setAnalogSteer(analogVal);
			physics->setTurnDirectionalInfluence(analogVal);
		}
	}

	// KEY RELEASED
	if (triggerLeft == 0.0 && triggerRight == 0.0) { // accel/reverse/brake
		input->setAnalogAccel(0.0f);
		input->setAnalogBrake(0.0f);
	}

	if (thumbLeftDeadZone == 0.0) { // left/right
		input->setAnalogSteer(0.0f);
	}
}

