/*
* Keyboard and Controller callback handler
*/
#include "VehicleController.h"

using namespace std;

XboxController::XboxController(PhysicsSystem* physicsSystem) : forwards(true) {
	this->physics = physicsSystem;
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

void XboxController::setButtonStateFromController(int controllerId) {
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
	bool X_button_pressed = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_X) != 0); // UNUSED FOR NOW
	bool START_button_pressed = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_START) != 0); // RESET

	if (X_button_pressed)
	{
		input->setAnalogHandbrake(1.0f);
	}
	else
	{
		input->setAnalogHandbrake(0.0f);
	}

	// Respawn player
	if (START_button_pressed)
	{
		physics->respawnPlayer(controllerId + 1);
	}
	//std::cout << physics->mVehiclePlayer1->computeForwardSpeed() << std::endl;

	float analogVal;
	if (thumbRightX <= 0) { // left
		physics->setViewDirectionalInfluence(thumbRightDeadZone);
	}
	if (thumbRightX > 0) { // right
		analogVal = -1.0 * thumbRightDeadZone;
		physics->setViewDirectionalInfluence(analogVal);
	}

	// KEY PRESSED
	//std::cout << physics->mVehiclePlayer1->mWheelsDynData.getWheelRotationSpeed(0) << std::endl;
	//if (physics->mVehiclePlayer1->mDriveDynData.mCurrentGear == snippetvehicle::PxVehicleGearsData::eNEUTRAL) {
	//	std::cout << "Neutral" << std::endl;
	//}
	//else if (physics->mVehiclePlayer1->mDriveDynData.mCurrentGear == snippetvehicle::PxVehicleGearsData::eFIRST) {
	//	std::cout << "FIRST" << std::endl;
	//}
	//else {
	//	std::cout << "REVERSE" << std::endl;
	//}
	//TODO: LOOK AT THIS FOR THE BRAKING
	//std::cout << "Left: " << triggerLeft << " Right: " << triggerRight << std::endl;
	if (triggerLeft > 0.1 && triggerRight > 0.1) { // brake
		//std::cout << "both pressed" << std::endl;
		// START OF REFACTOR
		//physics->mVehiclePlayer1->mDriveDynData.forceGearChange(snippetvehicle::PxVehicleGearsData::eNEUTRAL);
		analogVal = pow(triggerLeft / 255,4.0);
		input->setAnalogBrake(analogVal);

		// Should brake regardless of direction

		// END OF REFACTOR

		//physics->mVehiclePlayer1->mDriveDynData.setEngineRotationSpeed(0.f);
		//analogVal = triggerLeft / 255;
		//input->setAnalogBrake(analogVal);
	}
	else if (triggerRight > 0.1) { // Forward/Break when backwards
		//std::cout << "only right pressed" << std::endl;

		// START OF REFACTOR 
		// 
		// Accelerate if still or moving forwards
		if (physics->mVehiclePlayer1->mDriveDynData.mCurrentGear == snippetvehicle::PxVehicleGearsData::eNEUTRAL
			&& physics->mVehiclePlayer1->computeForwardSpeed() > -8) {
			physics->mVehiclePlayer1->mDriveDynData.forceGearChange(snippetvehicle::PxVehicleGearsData::eFIRST);
		}
		else if (physics->mVehiclePlayer1->mDriveDynData.mCurrentGear == snippetvehicle::PxVehicleGearsData::eREVERSE) {
			physics->mVehiclePlayer1->mDriveDynData.forceGearChange(snippetvehicle::PxVehicleGearsData::eNEUTRAL);
		}

		if (physics->mVehiclePlayer1->mDriveDynData.mCurrentGear == snippetvehicle::PxVehicleGearsData::eFIRST) { // Accelerate forward
			if (physics->mVehiclePlayer1->computeForwardSpeed() < 45)
			{
				analogVal = triggerRight / 255;
				input->setAnalogAccel(analogVal);
			}
			else
			{
				input->setAnalogAccel(0);
			}
		}
		else { //brake 
			analogVal = pow(triggerRight / 255, 4.0);
			input->setAnalogBrake(analogVal);
		}
		// END OF REFACTOR

		//if (physics->mVehiclePlayer1->mDriveDynData.mCurrentGear == snippetvehicle::PxVehicleGearsData::eREVERSE ||
		//	physics->mVehiclePlayer1->mDriveDynData.mCurrentGear == snippetvehicle::PxVehicleGearsData::eNEUTRAL)
		//	physics->mVehiclePlayer1->mDriveDynData.forceGearChange(snippetvehicle::PxVehicleGearsData::eFIRST);
		//
		//if (physics->mVehiclePlayer1->computeForwardSpeed() < -30)
		//{
		//	physics->mVehiclePlayer1->mDriveDynData.setEngineRotationSpeed(0.f);
		//	input->setAnalogBrake(1.f);
		//}
		//else if (physics->mVehiclePlayer1->computeForwardSpeed() > -30 && physics->mVehiclePlayer1->computeForwardSpeed() < -2)
		//{
		//	physics->mVehiclePlayer1->mWheelsDynData.setToRestState();
		//}
		//if (physics->mVehiclePlayer1->computeForwardSpeed() < 45)
		//{
		//	analogVal = triggerRight / 255;
		//	input->setAnalogAccel(analogVal);
		//}
		//else
		//{
		//	input->setAnalogAccel(0);
		//}
		//input->setAnalogBrake(0);
	}
	else if (triggerLeft > 0.1){// && triggerRight == 0.0) { // Reverse/Break when forward
		//std::cout << "only left pressed" << std::endl;
		// START OF REFACTOR
		if (physics->mVehiclePlayer1->mDriveDynData.mCurrentGear == snippetvehicle::PxVehicleGearsData::eNEUTRAL 
			&& physics->mVehiclePlayer1->computeForwardSpeed() < 8) {
			physics->mVehiclePlayer1->mDriveDynData.forceGearChange(snippetvehicle::PxVehicleGearsData::eREVERSE);
		}
		else if (physics->mVehiclePlayer1->mDriveDynData.mCurrentGear == snippetvehicle::PxVehicleGearsData::eFIRST) {
			physics->mVehiclePlayer1->mDriveDynData.forceGearChange(snippetvehicle::PxVehicleGearsData::eNEUTRAL);
		}

		//Brake if moving forward

		//Accelerate if moving in reverse
		if (physics->mVehiclePlayer1->mDriveDynData.mCurrentGear == snippetvehicle::PxVehicleGearsData::eREVERSE) { // Accelerate backwards
			if (physics->mVehiclePlayer1->computeForwardSpeed() > -30)
			{
				analogVal = pow(triggerLeft / 255,4.0);
				input->setAnalogAccel(analogVal);
			}
			else
			{
				input->setAnalogAccel(0);
			}
		}
		else { // Brake
			analogVal = triggerLeft / 255;
			input->setAnalogBrake(analogVal);
		}




		// END OF REFACTOR
		
		//if (physics->mVehiclePlayer1->mDriveDynData.mCurrentGear != snippetvehicle::PxVehicleGearsData::eREVERSE)
		//	physics->mVehiclePlayer1->mDriveDynData.forceGearChange(snippetvehicle::PxVehicleGearsData::eREVERSE);
		//if (physics->mVehiclePlayer1->computeForwardSpeed() > 50)
		//{
		//	//std::cout << "NORMAL BRAKE FORWARD" << std::endl;
		//	physics->mVehiclePlayer1->mDriveDynData.setEngineRotationSpeed(0.f);
		//	input->setAnalogBrake(1.f);
		//}
		//else if (physics->mVehiclePlayer1->computeForwardSpeed() < 50 && physics->mVehiclePlayer1->computeForwardSpeed() > 2)
		//{
		//	//std::cout << "HARD BRAKE FORWARD" << std::endl;
		//	physics->mVehiclePlayer1->mWheelsDynData.setToRestState();
		//}

		//if (abs(physics->mVehiclePlayer1->computeForwardSpeed()) < 20)
		//{
		//	analogVal = triggerLeft / 255;
		//	input->setAnalogAccel(analogVal);
		//}
		//else
		//{
		//	input->setAnalogAccel(0);
		//}
		//input->setAnalogBrake(0);
	}
	else
	{
		physics->mVehiclePlayer1->mDriveDynData.forceGearChange(snippetvehicle::PxVehicleGearsData::eNEUTRAL);	
		if (abs(physics->mVehiclePlayer1->computeForwardSpeed()) < 8)
		{
			physics->mVehiclePlayer1->mWheelsDynData.setToRestState();
		}
	}

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

	// KEY RELEASED
	if (triggerLeft == 0.0 && triggerRight == 0.0) { // accel/reverse/brake
		input->setAnalogAccel(0.0f);
		input->setAnalogBrake(0.0f);
	}

	if (thumbLeftDeadZone == 0.0) { // left/right
		input->setAnalogSteer(0.0f);
	}
}

