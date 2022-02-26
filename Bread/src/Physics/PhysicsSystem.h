#pragma once
#include <vector>

#include <iostream>

#include <PxPhysicsAPI.h>
#include <snippetvehiclecommon/SnippetVehicleSceneQuery.h>

#include "PhysicsEnums.h"

using namespace physx;
using namespace snippetvehicle;

class PhysicsSystem
{
public:

	PhysicsSystem();
	physx::PxTriangleMesh* cookKitchen();
	void initVehicleSDK();
	void initializeActors();

	PxRigidDynamic* createFoodBlock(const PxTransform& t, PxReal halfExtent, std::string name);
	PxRigidDynamic* createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity);

	void update(const float timestep);
	void updateFoodTransforms();
	void setAnalogInputs(bool input);

	void cleanupPhysics();

	PxVehicleDrive4W* mVehiclePlayer1;
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

	PxTriangleMesh* kitchenMesh;

	VehicleSceneQueryData* mVehicleSceneQueryData;
	PxBatchQuery* mBatchQuery;
	PxVehicleDrivableSurfaceToTireFrictionPairs* mFrictionPairs;
	PxRigidStatic* mGroundPlane;

	bool mIsVehicleInAir = true;
	bool useAnalogInputs = false;
}; 
