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

void XboxController::setButtonStateFromController(int controllerId) {
	physx::PxVehicleDrive4WRawInputData* input = &physics->mVehicleInputData;
	XINPUT_STATE state = getControllerState(controllerId);

	// Left Thumb
	float thumbLeftX = state.Gamepad.sThumbLX;
	float thumbLeftY = state.Gamepad.sThumbLY;
	float thumbLeftDeadZone = getDeadZone(thumbLeftX, thumbLeftY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);

	// Left trigger
	float triggerLeft = state.Gamepad.bLeftTrigger;

	// Right trigger
	float triggerRight = state.Gamepad.bRightTrigger;

	// Other buttons
	bool A_button_pressed = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0); // UNUSED FOR NOW

	// KEY PRESSED
	if (triggerRight > 0.1) { // forwards
		if (physics->mVehiclePlayer1->mDriveDynData.mCurrentGear == snippetvehicle::PxVehicleGearsData::eREVERSE ||
			physics->mVehiclePlayer1->mDriveDynData.mCurrentGear == snippetvehicle::PxVehicleGearsData::eNEUTRAL)
			physics->mVehiclePlayer1->mDriveDynData.forceGearChange(snippetvehicle::PxVehicleGearsData::eFIRST);
		input->setDigitalAccel(true);
		//input->setAnalogAccel(1.0f);
	}
	if (triggerLeft > 0.1 && triggerRight == 0.0) { // reverse
		if (physics->mVehiclePlayer1->mDriveDynData.mCurrentGear != snippetvehicle::PxVehicleGearsData::eREVERSE)
			physics->mVehiclePlayer1->mDriveDynData.forceGearChange(snippetvehicle::PxVehicleGearsData::eREVERSE);
		input->setDigitalAccel(true);
		//input->setAnalogAccel(1.0f);
	}
	if (triggerLeft > 0.1 && triggerRight > 0.1) { // brake
		input->setDigitalBrake(true);
		//input->setAnalogBrake(1.0f);
	}
	if (thumbLeftX <= 0 && thumbLeftDeadZone > 0.1) { // left
		input->setDigitalSteerRight(true);
		//input->setAnalogSteer(-1.0f);
	}
	if (thumbLeftX > 0 && thumbLeftDeadZone > 0.1) { // right
		input->setDigitalSteerLeft(true);
		//input->setAnalogSteer(-1.0f);
	}


	// KEY RELEASED
	if (triggerLeft == 0.0 && triggerRight == 0.0) { // accel/reverse/brake
		input->setDigitalAccel(false);
		input->setDigitalBrake(false);
		//input->setAnalogAccel(0.0f);
	}

	if (thumbLeftDeadZone == 0.0) { // left/right
		input->setDigitalSteerRight(false);
		input->setDigitalSteerLeft(false);
		//input->setAnalogSteer(0.0f);
	}
}

