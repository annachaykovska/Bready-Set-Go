#include <vector>
#include <memory>
#include <Windows.h>
#include <xinput.h>

#include "../Window.h"
#include "../Scene/Scene.h"
#include "PhysicsSystem.h"

#pragma comment(lib, "Xinput.lib") 
#pragma comment(lib, "Xinput9_1_0.lib")

extern Scene g_scene;
	

class XboxController {
public:
	XboxController(PhysicsSystem* physicsSystem);
	void checkControllers();
	void setButtonStateFromController(int controllerId);
	_XINPUT_STATE getControllerState(int controllerId);
	float getDeadZone(float x, float y, float deadzone);

private:
	PhysicsSystem* physics;
	bool forwards;
};

class MovementCallbacks : public CallbackInterface
{

public:

	MovementCallbacks(PhysicsSystem* physics) : physics(physics) { }

	virtual void keyCallback(int key, int scancode, int action, int mods)
	{
		physx::PxVehicleDrive4WRawInputData* input = &physics->mVehicleInputDataPlayer1; // Default to player 1

		// KEY PRESSED
		if (action == GLFW_PRESS)
		{
			switch (key)
			{
				// VEHICLE CONTROLS
			case GLFW_KEY_W:
				if (physics->mVehiclePlayer1->mDriveDynData.mCurrentGear == snippetvehicle::PxVehicleGearsData::eREVERSE ||
					physics->mVehiclePlayer1->mDriveDynData.mCurrentGear == snippetvehicle::PxVehicleGearsData::eNEUTRAL)
					physics->mVehiclePlayer1->mDriveDynData.forceGearChange(snippetvehicle::PxVehicleGearsData::eFIRST);
				input->setDigitalAccel(true);
				//input->setAnalogAccel(1.0f);
				break;

			case GLFW_KEY_S:
				if (physics->mVehiclePlayer1->mDriveDynData.mCurrentGear != snippetvehicle::PxVehicleGearsData::eREVERSE)
					physics->mVehiclePlayer1->mDriveDynData.forceGearChange(snippetvehicle::PxVehicleGearsData::eREVERSE);
				input->setDigitalAccel(true);
				//input->setAnalogAccel(1.0f);
				break;

			case GLFW_KEY_A:
				input->setDigitalSteerRight(true);
				//input->setAnalogSteer(-1.0f);
				break;

			case GLFW_KEY_D:
				input->setDigitalSteerLeft(true);
				//input->setAnalogSteer(1.0f);

				break;

			case GLFW_KEY_SPACE:
				input->setDigitalBrake(true);
				//input->setAnalogBrake(1.0f);
				break;
			}
		}

		// KEY RELEASED
		else if (action == GLFW_RELEASE)
		{
			switch (key)
			{
				// VEHICLE CONTROLS
			case GLFW_KEY_W:
				input->setDigitalAccel(false);
				//input->setAnalogAccel(0.0f);
				break;

			case GLFW_KEY_S:
				input->setDigitalAccel(false);
				//input->setAnalogAccel(0.0f);
				break;

			case GLFW_KEY_A:
				input->setDigitalSteerRight(false);
				//input->setAnalogSteer(0.0f);
				break;

			case GLFW_KEY_D:
				input->setDigitalSteerLeft(false);
				//input->setAnalogSteer(0.0f);
				break;

			case GLFW_KEY_SPACE:
				input->setDigitalBrake(false);
				//input->setAnalogBrake(0.0f);
				break;
			}
		}
	}

private:
	PhysicsSystem* physics;
};