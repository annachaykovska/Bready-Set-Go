#pragma once
#include <vector>

#include <PxPhysicsAPI.h>
#include <snippetvehiclecommon/SnippetVehicleSceneQuery.h>

#include "PhysicsEnums.h"

using namespace physx;
using namespace snippetvehicle;

class PhysicsSystem
{
public:

	PhysicsSystem();
	void initVehicleSDK();
	void initializeActors();
	PxRigidDynamic* createFoodBlock(const PxTransform& t, PxReal halfExtent);
	void update(const float timestep);
	void updateFoodTransforms();
	void startAccelerateForwardsMode();
	void startAccelerateReverseMode();
	void startBrakeMode();
	void startTurnHardLeftMode();
	void startTurnHardRightMode();
	void startHandbrakeTurnLeftMode();
	void startHandbrakeTurnRightMode();
	void releaseGas();
	void releaseSteering();
	void releaseAllControls();
	void cleanupPhysics();
	void keyPress(unsigned char key);
	void keyRelease(unsigned char key);
	PxRigidDynamic* createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity);

	// Public Entities
	PxVehicleDrive4W* mVehiclePlayer1; // TODO: Rename this?
	PxRigidDynamic* cheese;

	// Button State
	struct ButtonState {
		bool forwardsHeld = false;
		bool leftHeld = false;
		bool backwardsHeld = false;
		bool rightHeld = false;

		bool cameraForward = false;
		bool cameraBackward = false;
		bool cameraRight = false;
		bool cameraLeft = false;
		bool cameraUp = false;
		bool cameraDown = false;
	};

	
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
	PxF32 mVehicleModeLifetime = 0.2f;
	PxF32 mVehicleModeTimer = 0.0f;
	PxU32 mVehicleOrderProgress = 0;
	bool mVehicleOrderComplete = false;
	bool mMimicKeyInputs = false;
}; 
