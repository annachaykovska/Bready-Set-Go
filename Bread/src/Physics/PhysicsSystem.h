#pragma once
#include <vector>

#include <iostream>

#include <PxPhysicsAPI.h>
#include <snippetvehiclecommon/SnippetVehicleSceneQuery.h>

#include "PhysicsEnums.h"
#include <string.h>

using namespace physx;
using namespace snippetvehicle;

class PhysicsSystem
{
public:

	PhysicsSystem();
	void initVehicleSDK();
	void initializeActors();

	PxRigidDynamic* createFoodBlock(const PxTransform& t, PxReal halfExtent, std::string name);
	PxRigidDynamic* createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity);

	void update(const float timestep);
	void updateVehicle(PxVehicleDrive4W *player, bool &isVehicleInAir, std::string entityName, const float timestep);
	void updateFoodTransforms();
	void setAnalogInputs(bool input);

	void setViewDirectionalInfluence(float value);
	float getViewDirectionalInfluence();

	void cleanupPhysics();

	PxVehicleDrive4W* mVehiclePlayer1;
	PxVehicleDrive4W* mVehiclePlayer2;
	PxVehicleDrive4W* mVehiclePlayer3;
	PxVehicleDrive4W* mVehiclePlayer4;
	PxRigidDynamic* cheese;
	PxRigidDynamic* sausage;
	PxRigidDynamic* tomato;
	PxRigidDynamic* dough;

	PxVehicleDrive4WRawInputData mVehicleInputData;
	
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

	bool mIsVehicleInAirPlayer1 = true;
	bool mIsVehicleInAirPlayer2 = true;
	bool mIsVehicleInAirPlayer3 = true;
	bool mIsVehicleInAirPlayer4 = true;
	bool useAnalogInputs = false;

	float viewDirectionalInfluence = 0.f;
}; 
