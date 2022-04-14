/*
* Keyboard and Controller callback handler
*/
#include "VehicleController.h"

using namespace std;

XboxController::XboxController(PhysicsSystem* physicsSystem, UISystem* uiSystem, GameLoopManager* gameLoopManager) : forwards(true) {
	this->physics = physicsSystem;
	this->ui = uiSystem;
	this->gameLoop = gameLoopManager;
	this->useFlip1 = true;
	this->useFlip2 = true;
	this->useFlip3 = true;
	this->useFlip4 = true;
	this->y_held1 = false;
	this->y_held2 = false;
	this->y_held3 = false;
	this->y_held4 = false;
	this->b_held1 = false;
	this->b_held2 = false;
	this->b_held3 = false;
	this->b_held4 = false;
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

void XboxController::setButtonStateFromControllerMainMenu(int controllerId, AudioSource* menuSource) {
	XINPUT_STATE state = getControllerState(controllerId);

	// Left Thumb
	float thumbLeftX = state.Gamepad.sThumbLX;
	float thumbLeftY = state.Gamepad.sThumbLY;

	float thumbLeftDeadZone = getDeadZone(thumbLeftX, thumbLeftY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);

	// Other buttons
	bool A_button_pressed = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0); // accept choice
	bool B_button_pressed = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_B) != 0); // return

	float newTime = glfwGetTime();
	if (newTime - gameLoop->mainMenuTimeoutStart > gameLoop->mainMenuTimeoutLength) {
		gameLoop->mainMenuTimeoutStart = -1;
		if (A_button_pressed) {
			gameLoop->isMenuItemSelected = true; 
			menuSource->stop();
			menuSource->play("chop.wav");
		}
		if (B_button_pressed) {
			if (gameLoop->gameStage != GameLoopMode::MENU_START) {
				gameLoop->gameStage--;
				gameLoop->menuSelectionNumber = 1;
				gameLoop->mainMenuTimeoutStart = newTime;
				menuSource->stop();
				menuSource->play("chop.wav");
			}
		}
	}

	if (thumbLeftY > 0.0 && thumbLeftDeadZone > 0.1) {
		if (gameLoop->menuSelectionNumber == 2) {
			gameLoop->menuSelectionNumber = 1;
			menuSource->stop();
			menuSource->play("knife.wav");
		}
	}
	else if (thumbLeftY < 0.0 && thumbLeftDeadZone > 0.1) {
		if (gameLoop->menuSelectionNumber == 1) {
			gameLoop->menuSelectionNumber = 2;
			menuSource->stop();
			menuSource->play("knife.wav");
		}
	}	
}

void XboxController::setButtonStateFromControllerDriving(int controllerId, bool gameCompleted, AudioSource* menuSource) {
	// Get the correct input data for the controller
	physx::PxVehicleDrive4WRawInputData* input;
	physx::PxVehicleDrive4W* vehiclePhysics;
	auto player = g_scene.getEntity("player1");
	bool* y_held;
	bool* b_held;
	bool* useFlip;
	float* b_buttonTimeStart;
	Camera* camera;

	if (controllerId == 1) {
		input = &physics->mVehicleInputDataPlayer2;
		vehiclePhysics = physics->mVehiclePlayer2;
		y_held = &y_held2;
		b_held = &b_held2;
		useFlip = &useFlip2;
		b_buttonTimeStart = &b_buttonTimeStart2;
		player = g_scene.getEntity("player2");
		camera = g_scene.p2Camera;
	}
	else if (controllerId == 2) {
		input = &physics->mVehicleInputDataPlayer3;
		vehiclePhysics = physics->mVehiclePlayer3;
		y_held = &y_held3;
		b_held = &b_held3;
		useFlip = &useFlip3;
		b_buttonTimeStart = &b_buttonTimeStart3;
		player = g_scene.getEntity("player3");
		camera = g_scene.p3Camera;
	}
	else if (controllerId == 3) {
		input = &physics->mVehicleInputDataPlayer4;
		vehiclePhysics = physics->mVehiclePlayer4;
		y_held = &y_held4;
		b_held = &b_held4;
		useFlip = &useFlip4;
		b_buttonTimeStart = &b_buttonTimeStart4;
		player = g_scene.getEntity("player4");
		camera = g_scene.p4Camera;
	}
	else {
		input = &physics->mVehicleInputDataPlayer1; // Defaults to player 1
		vehiclePhysics = physics->mVehiclePlayer1;
		y_held = &y_held1;
		b_held = &b_held1;
		useFlip = &useFlip1;
		b_buttonTimeStart = &b_buttonTimeStart1;
		camera = g_scene.p1Camera;
	}

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
	bool B_button_pressed = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_B) != 0); // Unflip vehicle
	bool START_button_pressed = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_START) != 0); // RESET
	bool BACK_button_pressed = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) != 0); // pause

	if (Y_button_pressed && !*y_held) { // y just pressed
		physics->magnetCheckStealing(controllerId+1, Y_button_pressed, !*y_held);
		*y_held = true;
	}
	else if (Y_button_pressed) { // y held
		physics->magnetCheckStealing(controllerId+1, Y_button_pressed, !*y_held);
	}
	else { // y not pressed
		physics->magnetCheckStealing(controllerId+1, Y_button_pressed, !*y_held);
		*y_held = false;
	}

	// Update the vibration 
	if (player->magnetStatus == 2) {
		vibrateController(controllerId, true, 1);
	}
	else if (player->magnetStatus == 3)
		vibrateController(controllerId, true, 2);
	else {
		vibrateController(controllerId, false, 0);
	}

	// Handbrake
	if (X_button_pressed)
	{
		input->setAnalogHandbrake(1.0f);
	}
	else
	{
		input->setAnalogHandbrake(0.0f);
	}

	// Flip car
	float currentTime = glfwGetTime();
	if (B_button_pressed) {
		if (*b_held && *useFlip) {
			player->unflipTimer = currentTime - *b_buttonTimeStart;
			if (currentTime - *b_buttonTimeStart > 2) {
				*useFlip = false;
				*b_held = false;
				player->unflipTimer = 0;
				physics->respawnPlayerInPlace(controllerId + 1);
				*b_buttonTimeStart = -1;
				//printf("RESPAWN\n");
			}
		}
		else {
			player->unflipTimer = 0;
			*b_held = true;
			*b_buttonTimeStart = currentTime;
		}
	}
	else {
		*useFlip = true;
		player->unflipTimer = 0;
		*b_held = false;
	}

	// Pause menu
	if (currentTime - gameLoop->showPauseMenuTimeoutStart > gameLoop->showPauseMenuTimeoutLength) {
		gameLoop->showPauseMenuTimeoutStart = -1;
		if (START_button_pressed) {
			gameLoop->showPauseMenuTimeoutStart = currentTime;
			gameLoop->showPauseMenu = !gameLoop->showPauseMenu;
			menuSource->stop();
			menuSource->play("chop.wav");
			if (gameLoop->showPauseMenu) {
				gameLoop->isPaused = true;
			}
			else {
				gameLoop->isPauseMenuItemSelected = true;
				gameLoop->pauseMenuSelection = 1; // Pretend continue was pressed
			}
		}
	}

	if (A_button_pressed && gameLoop->showPauseMenu) {
		gameLoop->showPauseMenuTimeoutStart = currentTime;
		gameLoop->isPauseMenuItemSelected = true;
		menuSource->stop();
		menuSource->play("chop.wav");
	}

	// Accept exit conditions back to the menu
	if (currentTime - gameLoop->mainMenuTimeoutStart > gameLoop->mainMenuTimeoutLength) {
		gameLoop->mainMenuTimeoutStart = -1;
		// Last check is to prevent the pause menu from going back immediately
		if (A_button_pressed && gameCompleted && currentTime - gameLoop->showPauseMenuTimeoutStart > gameLoop->showPauseMenuTimeoutLength) {
			gameLoop->showPauseMenuTimeoutStart = -1;
			gameLoop->isBackToMenuSelected = true;
		}
	}

	// Respawn player
	if (BACK_button_pressed)
	{
		physics->respawnPlayer(controllerId + 1);
	}

	float analogVal;
	float analogVal2;
	if (thumbRightX <= 0) { // left
		camera->viewDirectionalInfluence = thumbRightDeadZone;
	}
	if (thumbRightX > 0) { // right
		analogVal = -1.0 * thumbRightDeadZone;
		camera->viewDirectionalInfluence = analogVal;
	}

	// TRIGGER PRESSED
	if (triggerLeft > 0.1 && triggerRight > 0.1) { // brake
		if (vehiclePhysics->computeForwardSpeed()<0) {
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
		if (vehiclePhysics->mDriveDynData.mCurrentGear != snippetvehicle::PxVehicleGearsData::eFIRST)
			vehiclePhysics->mDriveDynData.forceGearChange(snippetvehicle::PxVehicleGearsData::eFIRST);
		
		if (vehiclePhysics->computeForwardSpeed() < -30)
		{
			vehiclePhysics->mDriveDynData.setEngineRotationSpeed(0.f);
			input->setAnalogBrake(1.f);
		}
		else if (vehiclePhysics->computeForwardSpeed() < 45)
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

		if (vehiclePhysics->mDriveDynData.mCurrentGear != snippetvehicle::PxVehicleGearsData::eREVERSE)
			vehiclePhysics->mDriveDynData.forceGearChange(snippetvehicle::PxVehicleGearsData::eREVERSE);
		if (vehiclePhysics->computeForwardSpeed() > 50)
		{
			vehiclePhysics->mDriveDynData.setEngineRotationSpeed(0.f);
			input->setAnalogBrake(1.f);
		}
		else if (abs(vehiclePhysics->computeForwardSpeed()) < 20)
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
		vehiclePhysics->mDriveDynData.forceGearChange(snippetvehicle::PxVehicleGearsData::eNEUTRAL);
		if (abs(vehiclePhysics->computeForwardSpeed()) < 1)
		{
			vehiclePhysics->mWheelsDynData.setToRestState();
		}
	}

	if (gameLoop->showPauseMenu) {
		if (thumbLeftY > 0.0 && thumbLeftDeadZone > 0.1) {
			if (gameLoop->pauseMenuSelection == 2) {
				gameLoop->pauseMenuSelection = 1;
				menuSource->stop();
				menuSource->play("knife.wav");
			}

		}
		else if (thumbLeftY < 0.0 && thumbLeftDeadZone > 0.1) {
			if (gameLoop->pauseMenuSelection == 1) {
				gameLoop->pauseMenuSelection = 2;
				menuSource->stop();
				menuSource->play("knife.wav");
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
			camera->turnDirectionalInfluence = thumbLeftDeadZone;

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
			camera->turnDirectionalInfluence = analogVal;
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

int XboxController::getNumberConnectedControllers() {
	int numControllers = 0;

	for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
	{
		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));

		if (XInputGetState(i, &state) == ERROR_SUCCESS) {
			numControllers++;
		}
	}
	//printf("NUM CONTROLLER: %d\n", numControllers);
	return numControllers;
}

void XboxController::vibrateController(int controllerId, bool vibrate, int pattern) {
	XINPUT_VIBRATION vibration;
	ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
	if (vibrate) {
		if (pattern == 1) {
			vibration.wLeftMotorSpeed = 14000; // use any value between 0-65535 here
			vibration.wRightMotorSpeed = 15000; // use any value between 0-65535 here
		}
		else if (pattern == 2) {
			vibration.wLeftMotorSpeed = 65535; // use any value between 0-65535 here
			vibration.wRightMotorSpeed = 65535; // use any value between 0-65535 here
		}
	} 
	else {
		vibration.wLeftMotorSpeed = 0; // use any value between 0-65535 here
		vibration.wRightMotorSpeed = 0; // use any value between 0-65535 here
	}
	XInputSetState(controllerId, &vibration);
}
