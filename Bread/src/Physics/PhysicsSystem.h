#pragma once
#include <vector>
#include <string.h>
#include <iostream>

#include <PxPhysicsAPI.h>
#include <snippetvehiclecommon/SnippetVehicleSceneQuery.h>
#include <glm/glm.hpp>

#include "PhysicsEnums.h"

using namespace physx;
using namespace snippetvehicle;

class Entity;

class PhysicsSystem
{
public:

	PhysicsSystem();
	void initialize();
	void cookKitchen();
	void initVehicleSDK();
	void initializeActors();
	void randomizeIngredientLocations();

	PxRigidDynamic* createFoodBlock(const PxTransform& t, float x, float y, float z, std::string name);
	PxRigidDynamic* createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity);
	PxRigidDynamic* createObstacle(const PxTransform& t, PxReal halfExtent, std::string name);
	void update(const float timestep, int gameStage);
	void updateVehicle(PxVehicleDrive4W *player, bool &isVehicleInAir, PxVehicleDrive4WRawInputData& inputData, std::string entityName, const float timestep, int gameStage);
	void updateFoodTransforms(bool setAllVisible = false);
	void setAnalogInputs(bool input);

	float getPlayerSpeed(int playerNumber);
	float getPlayerSidewaysSpeed(int playerNumber);
	bool getIsVehicleInAir(int playerNumber);
	void resetOutOfBoundsObjects();
	void respawnPlayer(int playerNumber);
	void respawnPlayerInPlace(int playerNumber);
	void playerCollisionRaycast(Entity* firstActor, PxVehicleDrive4W* firstVehicle, Entity* secondActor, PxVehicleDrive4W* secondVehicle);

	// Functions for magnet ability
	void magnet(int stealer_id);

	// Sets entity magnet status
	// 0 = in cooldown
	// 1 = ready to steal, not close to target
	// 2 = ready to steal, close to target
	// 3 = tethered, and stealing from target
	void magnetCheckStealing(int stealer_id, bool steal_button_held, bool steal_button_just_pressed); 

	// Updates the victims that can be stolen from (TODO: SHOULD BE PRIVATE)
	void checkVictims(Entity* stealer, std::vector<Entity*>& victims, float currentTime);
	// Make victims list ( TODO: SHOULD BE PRIVATE)
	void makeVictimsList(int stealer_id, std::vector<Entity*>& victims);

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
	//PxRigidDynamic* soupbase;
	//PxRigidDynamic* pumpkin;

	PxVehicleDrive4WRawInputData mVehicleInputDataPlayer1;
	PxVehicleDrive4WRawInputData mVehicleInputDataPlayer2;
	PxVehicleDrive4WRawInputData mVehicleInputDataPlayer3;
	PxVehicleDrive4WRawInputData mVehicleInputDataPlayer4;
	
	// Start of tweaking variables
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
	
	// End of tweaking variables

	bool p1CameraHit;
	glm::vec3 p1CameraHitPos;
	bool p2CameraHit;
	glm::vec3 p2CameraHitPos;
	bool p3CameraHit;
	glm::vec3 p3CameraHitPos;
	bool p4CameraHit;
	glm::vec3 p4CameraHitPos;

private:

	void raycastCamera(physx::PxVehicleDrive4W* vehicle, std::string name);

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

	bool mImpulseAppliedPlayer1 = false;
	bool mImpulseAppliedPlayer2 = false;
	bool mImpulseAppliedPlayer3 = false;
	bool mImpulseAppliedPlayer4 = false;
};