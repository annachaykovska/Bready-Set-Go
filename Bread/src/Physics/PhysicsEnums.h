#pragma once
#include <vector>

#include <PxPhysicsAPI.h>

using namespace physx;


enum DriveMode
{
	eDRIVE_MODE_ACCEL_FORWARDS = 0,
	eDRIVE_MODE_ACCEL_REVERSE,
	eDRIVE_MODE_HARD_TURN_LEFT,
	eDRIVE_MODE_HANDBRAKE_TURN_LEFT,
	eDRIVE_MODE_HARD_TURN_RIGHT,
	eDRIVE_MODE_HANDBRAKE_TURN_RIGHT,
	eDRIVE_MODE_BRAKE,
	eDRIVE_MODE_NONE
};

// Button State
struct ButtonState {
	bool forwardsHeld = false;
	bool leftHeld = false;
	bool backwardsHeld = false;
	bool rightHeld = false;
	bool brakeHeld = false;

	bool cameraForward = false;
	bool cameraBackward = false;
	bool cameraRight = false;
	bool cameraLeft = false;
	bool cameraUp = false;
	bool cameraDown = false;
};
