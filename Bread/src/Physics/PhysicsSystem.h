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
	void initialize();
	void cookKitchen();
	void initVehicleSDK();
	void initializeActors();

	PxRigidDynamic* createFoodBlock(const PxTransform& t, PxReal halfExtent, std::string name);
	PxRigidDynamic* createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity);

	void update(const float timestep);
	void updateVehicle(PxVehicleDrive4W *player, bool &isVehicleInAir, PxVehicleDrive4WRawInputData& inputData, std::string entityName, const float timestep);
	void updateFoodTransforms();
	void setAnalogInputs(bool input);

	void setViewDirectionalInfluence(float value);
	float getViewDirectionalInfluence();
	void setTurnDirectionalInfluence(float value);
	float getTurnDirectionalInfluence();

	void cookieCutter(PxTriangleMesh& meshDesc, PxDefaultMemoryOutputStream& writeBuffer, PxTriangleMeshCookingResult::Enum& result);

	void cleanupPhysics();

	PxVehicleDrive4W* mVehiclePlayer1;
	PxVehicleDrive4W* mVehiclePlayer2;
	PxVehicleDrive4W* mVehiclePlayer3;
	PxVehicleDrive4W* mVehiclePlayer4;
	PxRigidDynamic* cheese;
	PxRigidDynamic* sausage;
	PxRigidDynamic* tomato;
	PxRigidDynamic* dough;

	PxVehicleDrive4WRawInputData mVehicleInputDataPlayer1;
	PxVehicleDrive4WRawInputData mVehicleInputDataPlayer2;
	PxVehicleDrive4WRawInputData mVehicleInputDataPlayer3;
	PxVehicleDrive4WRawInputData mVehicleInputDataPlayer4;
	
private:

	float mAccumulator;

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
	PxRigidStatic* kitchen;
	std::vector<physx::PxVec3>* kitchenVerts;
	std::vector<physx::PxU32>* kitchenIndices;

	VehicleSceneQueryData* mVehicleSceneQueryData;
	PxBatchQuery* mBatchQuery;
	PxVehicleDrivableSurfaceToTireFrictionPairs* mFrictionPairs;
	PxRigidStatic* mGroundPlane;

	bool mIsVehicleInAirPlayer1 = true;
	bool mIsVehicleInAirPlayer2 = true;
	bool mIsVehicleInAirPlayer3 = true;
	bool mIsVehicleInAirPlayer4 = true;
	bool useAnalogInputs = false;

	float viewDirectionalInfluence;
	float turnDirectionalInfluence;

	bool mImpulseAppliedPlayer1 = false;
	PxVec3 mStoredVelocityPlayer1 = PxVec3(0.0f);
}; 

