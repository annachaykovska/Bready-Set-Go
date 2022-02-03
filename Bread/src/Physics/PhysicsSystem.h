#pragma once

#include <PxPhysicsAPI.h>
#include <snippetvehiclecommon/SnippetVehicleSceneQuery.h>

using namespace physx;
using namespace snippetvehicle;

class PhysicsSystem
{
public:

	PhysicsSystem();
	void initVehicleSDK();
	void update(const float timeStep);
	void startAccelerateForwardsMode();
	void startAccelerateReverseMode();
	void startBrakeMode();
	void startTurnHardLeftMode();
	void startTurnHardRightMode();
	void startHandbrakeTurnLeftMode();
	void startHandbrakeTurnRightMode();
	void releaseAllControls();
	// void incrementDrivingMode(const PxF32 timestep);
	// void stepPhysics();
	// void cleanupPhysics();
	// void keyPress();

	// Public Variables
	PxVehicleDrive4W* mVehicle4W; // TODO: Rename this?

private:
	PxDefaultErrorCallback mDefaultErrorCallback;
	PxDefaultAllocator mDefaultAllocatorCallback;
	PxFoundation* mFoundation;
	PxPhysics* mPhysics;
	PxDefaultCpuDispatcher* mDispatcher;
	PxScene* mScene;
	PxCooking* mCooking;
	PxMaterial* mMaterial;
	PxPvd* mPvd;
	VehicleSceneQueryData* mVehicleSceneQueryData;
	PxBatchQuery* mBatchQuery;
	PxVehicleDrivableSurfaceToTireFrictionPairs* mFrictionPairs;
	PxRigidStatic* mGroundPlane;
	bool mIsVehicleInAir = true;
	PxVehicleDrive4WRawInputData mVehicleInputData;
	PxF32 mVehicleModeLifetime = 4.0f;
	PxF32 mVehicleModeTimer = 0.0f;
	PxU32 mVehicleOrderProgress = 0;
	bool mVehicleOrderComplete = false;
	bool mMimicKeyInputs = false;
}; 