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
	PxRigidDynamic* createObstacle(const PxTransform& t, PxReal halfExtent, std::string name);
	void update(const float timestep);
	void updateVehicle(PxVehicleDrive4W *player, bool &isVehicleInAir, PxVehicleDrive4WRawInputData& inputData, std::string entityName, const float timestep);
	void updateFoodTransforms();
	void setAnalogInputs(bool input);

	void setViewDirectionalInfluence(float value);
	float getViewDirectionalInfluence();
	void setTurnDirectionalInfluence(float value);
	float getTurnDirectionalInfluence();

	float getPlayerSpeed(int playerNumber);
	bool getIsVehicleInAir(int playerNumber);

	void respawnPlayer(int playerNumber);
	void playerCollisionRaycast(Entity* firstActor, PxVehicleDrive4W* firstVehicle, Entity* secondActor, PxVehicleDrive4W* secondVehicle);

	void cleanupPhysics();

	PxVehicleDrive4W* mVehiclePlayer1;
	PxVehicleDrive4W* mVehiclePlayer2;
	PxVehicleDrive4W* mVehiclePlayer3;
	PxVehicleDrive4W* mVehiclePlayer4;

	PxRigidDynamic* cheese;
	PxRigidDynamic* sausage;
	PxRigidDynamic* tomato;
	PxRigidDynamic* dough;
	PxRigidDynamic* carrot;
	PxRigidDynamic* lettuce;
	PxRigidDynamic* parsnip;
	PxRigidDynamic* rice;
	PxRigidDynamic* egg;
	PxRigidDynamic* chicken;
	PxRigidDynamic* peas;
	PxRigidDynamic* soupbase;
	PxRigidDynamic* pumpkin;


	PxVehicleDrive4WRawInputData mVehicleInputDataPlayer1;
	PxVehicleDrive4WRawInputData mVehicleInputDataPlayer2;
	PxVehicleDrive4WRawInputData mVehicleInputDataPlayer3;
	PxVehicleDrive4WRawInputData mVehicleInputDataPlayer4;
	
	// For Tweaking
	void updateCar();

	float chassis_mass;
	float chassis_moi_y;

	float wheel_mass;
	float wheel_moi;
	float wheel_damping_rate;
	float max_hand_brake_torque;
	float max_steer;

	float camber_at_rest;
	float spring_strength;
	float max_compression;
	float max_droop;

	//float chassis_moi[3];

	float peak_torque;
	float max_omega;
	float max_brake_torque;

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

	PxTransform startTransformPlayer1;
	PxTransform startTransformPlayer2;
	PxTransform startTransformPlayer3;
	PxTransform startTransformPlayer4;

	bool mIsVehicleInAirPlayer1 = true;
	bool mIsVehicleInAirPlayer2 = true;
	bool mIsVehicleInAirPlayer3 = true;
	bool mIsVehicleInAirPlayer4 = true;
	bool useAnalogInputs = false;

	float viewDirectionalInfluence;
	float turnDirectionalInfluence;

	bool mImpulseAppliedPlayer1 = false;
	bool mImpulseAppliedPlayer2 = false;
	bool mImpulseAppliedPlayer3 = false;
	bool mImpulseAppliedPlayer4 = false;
}; 

