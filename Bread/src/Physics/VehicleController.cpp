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

void XboxController::getDeadZone(float x, float y, float deadzone) {
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
}

void XboxController::setButtonStateFromController(int controllerId) {
	XINPUT_STATE state = getControllerState(controllerId);

	// Left Thumb
	float thumbLeftX = state.Gamepad.sThumbLX;
	float thumbLeftY = state.Gamepad.sThumbLY;
	// getDeadZone(thumbLeftX, thumbLeftY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);

	// Left trigger
	float triggerLeft = state.Gamepad.bLeftTrigger;

	// Right trigger
	float triggerRight = state.Gamepad.bRightTrigger;

	// Other buttons
	bool A_button_pressed = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0); // Enter?
	bool B_button_pressed = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_B) != 0); // idk

	/*printf("A: %d\n", A_button_pressed);
	printf("Thumb LX: %f\n", thumbLeftX);
	printf("Thumb LY: %f\n", thumbLeftY);
	printf("Trigger L: %f\n", triggerLeft);
	printf("Trigger R: %f\n\n", triggerRight);*/
}
