/*
* Keyboard and Controller callback handler
*/
#include "VehicleController.h"

using namespace std;

XboxController::XboxController(PhysicsSystem* physicsSystem) {
	this->physics = physicsSystem;
}

void XboxController::setControllers() {
	int controllerId = -1;

	for (DWORD i = 0; i < XUSER_MAX_COUNT && controllerId == -1; i++)
	{
		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));

		if (XInputGetState(i, &state) == ERROR_SUCCESS) {
			controllerId = i;
			
		}
	}
	printf("The controller ID: %d", controllerId);
}

_XINPUT_STATE XboxController::getControllerState(int controllerId) {
	XINPUT_STATE state;
	ZeroMemory(&state, sizeof(XINPUT_STATE));
	DWORD dwResult;

	dwResult = XInputGetState(controllerId, &state);
	return state;
}

void XboxController::setButtonStateFromController() {
	int i = 0;
	XINPUT_STATE state = getControllerState(i);


	bool A_button_pressed = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0); // Enter?
	bool B_button_pressed = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0); // idk


	float LX = state.Gamepad.sThumbLX;
	float LY = state.Gamepad.sThumbLY;

	// LEFT THUMB ------------------------------------------------------------------------------------
	//determine how far the controller is pushed
	float magnitude = sqrt(LX * LX + LY * LY);

	//determine the direction the controller is pushed
	float normalizedLX = LX / magnitude;
	float normalizedLY = LY / magnitude;

	float normalizedMagnitude = 0;

	//check if the controller is outside a circular dead zone
	if (magnitude > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
	{
		//clip the magnitude at its expected maximum value
		if (magnitude > 32767) magnitude = 32767;

		//adjust magnitude relative to the end of the dead zone
		magnitude -= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;

		//optionally normalize the magnitude with respect to its expected range
		//giving a magnitude value of 0.0 to 1.0
		normalizedMagnitude = magnitude / (32767 - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
	}
	else //if the controller is in the deadzone zero out the magnitude
	{
		magnitude = 0.0;
		normalizedMagnitude = 0.0;
	}

	printf("MAG:%f\tNMAG:%f\n", magnitude, normalizedMagnitude);

	//repeat for right thumb stick
}
