#include <cassert>

#include <glm/glm.hpp>

#include <snippetvehiclecommon/SnippetVehicleCreate.h>
#include <snippetvehiclecommon/SnippetVehicleFilterShader.h>
#include <snippetvehiclecommon/SnippetVehicleTireFriction.h>
#include <snippetutils/SnippetUtils.h>

#include "PhysicsSystem.h"
#include "CollisionCallback.h"
#include "../Scene/Scene.h"
#include "../Transform.h"
#include "../Scene/Entity.h"

using namespace snippetvehicle;
using namespace physx;

extern Scene g_scene;

CollisionCallback gCollisionCallback;

PxF32 gSteerVsForwardSpeedData[2 * 8] =
{
	0.0f,		0.75f,
	5.0f,		0.75f,
	30.0f,		0.125f,
	120.0f,		0.1f,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32
};
PxFixedSizeLookupTable<8> gSteerVsForwardSpeedTable(gSteerVsForwardSpeedData, 4);

PxVehicleKeySmoothingData gKeySmoothingData =
{
	{
		6.0f,	//rise rate eANALOG_INPUT_ACCEL
		6.0f,	//rise rate eANALOG_INPUT_BRAKE		
		6.0f,	//rise rate eANALOG_INPUT_HANDBRAKE	
		2.5f,	//rise rate eANALOG_INPUT_STEER_LEFT
		2.5f,	//rise rate eANALOG_INPUT_STEER_RIGHT
	},
	{
		10.0f,	//fall rate eANALOG_INPUT_ACCEL
		10.0f,	//fall rate eANALOG_INPUT_BRAKE		
		10.0f,	//fall rate eANALOG_INPUT_HANDBRAKE	
		5.0f,	//fall rate eANALOG_INPUT_STEER_LEFT
		5.0f	//fall rate eANALOG_INPUT_STEER_RIGHT
	}
};

PxVehiclePadSmoothingData gPadSmoothingData =
{
	{
		6.0f,	//rise rate eANALOG_INPUT_ACCEL
		6.0f,	//rise rate eANALOG_INPUT_BRAKE		
		6.0f,	//rise rate eANALOG_INPUT_HANDBRAKE	
		2.5f,	//rise rate eANALOG_INPUT_STEER_LEFT
		2.5f,	//rise rate eANALOG_INPUT_STEER_RIGHT
	},
	{
		10.0f,	//fall rate eANALOG_INPUT_ACCEL
		10.0f,	//fall rate eANALOG_INPUT_BRAKE		
		10.0f,	//fall rate eANALOG_INPUT_HANDBRAKE	
		5.0f,	//fall rate eANALOG_INPUT_STEER_LEFT
		5.0f	//fall rate eANALOG_INPUT_STEER_RIGHT
	}
};


VehicleDesc initVehicleDesc(PxMaterial* mMaterial)
{
	//Set up the chassis mass, dimensions, moment of inertia, and center of mass offset.
	//The moment of inertia is just the moment of inertia of a cuboid but modified for easier steering.
	//Center of mass offset is 0.65m above the base of the chassis and 0.25m towards the front.
	const PxF32 chassisMass = 1500.0f;
	const PxVec3 chassisDims(2.5f, 2.0f, 5.0f);
	const PxVec3 chassisMOI
	((chassisDims.y * chassisDims.y + chassisDims.z * chassisDims.z) * chassisMass / 12.0f,
		(chassisDims.x * chassisDims.x + chassisDims.z * chassisDims.z) * 0.8f * chassisMass / 12.0f,
		(chassisDims.x * chassisDims.x + chassisDims.y * chassisDims.y) * chassisMass / 12.0f);
	const PxVec3 chassisCMOffset(0.0f, -chassisDims.y * 0.5f + 0.65f, 0.25f);

	//Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
	//Moment of inertia is just the moment of inertia of a cylinder.
	const PxF32 wheelMass = 20.0f;
	const PxF32 wheelRadius = 0.5f;
	const PxF32 wheelWidth = 0.4f;
	const PxF32 wheelMOI = 0.3f * wheelMass * wheelRadius * wheelRadius;
	const PxU32 nbWheels = 4;

	VehicleDesc vehicleDesc;

	vehicleDesc.chassisMass = chassisMass;
	vehicleDesc.chassisDims = chassisDims;
	vehicleDesc.chassisMOI = chassisMOI;
	vehicleDesc.chassisCMOffset = chassisCMOffset;
	vehicleDesc.chassisMaterial = mMaterial;
	vehicleDesc.chassisSimFilterData = PxFilterData(COLLISION_FLAG_CHASSIS, COLLISION_FLAG_CHASSIS_AGAINST, 0, 0);

	vehicleDesc.wheelMass = wheelMass;
	vehicleDesc.wheelRadius = wheelRadius;
	vehicleDesc.wheelWidth = wheelWidth;
	vehicleDesc.wheelMOI = wheelMOI;
	vehicleDesc.numWheels = nbWheels;
	vehicleDesc.wheelMaterial = mMaterial;
	vehicleDesc.chassisSimFilterData = PxFilterData(COLLISION_FLAG_WHEEL, COLLISION_FLAG_WHEEL_AGAINST, 0, 0);

	return vehicleDesc;
}

PxRigidDynamic* PhysicsSystem::createFoodBlock(const PxTransform& t, PxReal halfExtent, std::string name)
{
	PxShape* shape = mPhysics->createShape(PxBoxGeometry(halfExtent, halfExtent, halfExtent), *mMaterial);
	PxFilterData cheeseFilter(COLLISION_FLAG_FOOD, COLLISION_FLAG_FOOD_AGAINST, 0, 0);
	shape->setSimulationFilterData(cheeseFilter);

	PxTransform localTm(PxVec3(10, 2, 10) * halfExtent);
	PxRigidDynamic* body = mPhysics->createRigidDynamic(t.transform(localTm));
	body->attachShape(*shape);
	PxRigidBodyExt::updateMassAndInertia(*body, 0.1f);

	// Set physx actor name
	body->setName(name.c_str()); // TODO one or both of these are not working correctly?

	// Attach the entity to the physx actor
	void* vp = static_cast<void*>(new std::string(name));
	body->userData = vp;

	mScene->addActor(*body);
	shape->release();

	return body;
}

PxRigidDynamic* PhysicsSystem::createObstacle(const PxTransform& t, PxReal halfExtent, std::string name)
{
	PxShape* shape = mPhysics->createShape(PxBoxGeometry(halfExtent, halfExtent, halfExtent), *mMaterial);
	PxFilterData cheeseFilter(COLLISION_FLAG_FOOD, COLLISION_FLAG_FOOD_AGAINST, 0, 0);
	shape->setSimulationFilterData(cheeseFilter);

	PxTransform localTm(PxVec3(0, 2, 0) * halfExtent);
	PxRigidDynamic* body = mPhysics->createRigidDynamic(t.transform(localTm));
	body->attachShape(*shape);
	PxRigidBodyExt::updateMassAndInertia(*body, 100.f);

	// Set physx actor name
	body->setName(name.c_str()); // TODO one or both of these are not working correctly?

	// Attach the entity to the physx actor
	void* vp = static_cast<void*>(new std::string(name));
	body->userData = vp;

	mScene->addActor(*body);
	shape->release();

	return body;
}

void PhysicsSystem::initializeActors() {
	// GROUND ---------------------------------------------------------------------------------------------------------------------
	// Add the ground plane to drive on
	PxFilterData groundPlaneSimFilterData(COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST, 0, 0);
	this->mGroundPlane = createDrivablePlane(groundPlaneSimFilterData, mMaterial, mPhysics);
	mScene->addActor(*mGroundPlane);

	// PLAYERS ---------------------------------------------------------------------------------------------------------------------
	//Create a vehicle that will drive on the plane.
	// PLAYER 1
	VehicleDesc vehicleDesc = initVehicleDesc(this->mMaterial);
	mVehiclePlayer1 = createVehicle4W(vehicleDesc, mPhysics, mCooking);
	PxTransform startTransform(PxVec3(0, (vehicleDesc.chassisDims.y * 0.5f + vehicleDesc.wheelRadius + 1.0f), 0), PxQuat(PxIdentity));
	mVehiclePlayer1->getRigidDynamicActor()->setGlobalPose(startTransform);

	// Set physx actor name
	mVehiclePlayer1->getRigidDynamicActor()->setName("player1"); // TODO one or both of these are not working correctly?
	
	// Attach the entity to the physx actor
	Entity* player1 = g_scene.getEntity("player1");
	void* vp = static_cast<void*>(new std::string("player1"));
	mVehiclePlayer1->getRigidDynamicActor()->userData = vp;

	mScene->addActor(*mVehiclePlayer1->getRigidDynamicActor());

	// FOOD ITEMS ---------------------------------------------------------------------------------------------------------------------
	
	// Note: Physx actor name must match Entity name
	// CHEESE
	float halfExtent = 1.0f;
	PxTransform cheeseTransform(PxVec3(0, 0, 30));
	this->cheese = createFoodBlock(cheeseTransform, halfExtent, "cheese");

	// SAUSAGE
	PxTransform sausageTransform(PxVec3(0, 0, -30));
	this->sausage = createFoodBlock(sausageTransform, halfExtent, "sausage");

	// TOMATO
	PxTransform tomatoTransform(PxVec3(30, 0, 0));
	this->tomato = createFoodBlock(tomatoTransform, halfExtent, "tomato");

	// DOUGH
	PxTransform doughTransform(PxVec3(-30, 0, 0));
	this->dough = createFoodBlock(doughTransform, halfExtent, "dough");

	// DEMO OBSTACLE
	PxTransform demoObstacleTransform(PxVec3(30, 0, -30));
	this->demoObstacle = createObstacle(demoObstacleTransform, 10.f, "demoObstacle");
}

PhysicsSystem::PhysicsSystem()
{
	this->mCooking = NULL;

	// Foundation
	this->mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, mDefaultAllocatorCallback, mDefaultErrorCallback);
	if (!mFoundation) { std::cout << "PxCreateFoundation failed!" << std::endl; assert(mFoundation); }

	// PVD
	this->mPvd = PxCreatePvd(*mFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	mPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	// Physics
	this->mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, PxTolerancesScale(), true, mPvd);
	initVehicleSDK();

	// Dispatcher
	PxU32 numWorkers = 1;
	this->mDispatcher = PxDefaultCpuDispatcherCreate(numWorkers);

	// Physx scene
	PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	sceneDesc.cpuDispatcher = this->mDispatcher;
	sceneDesc.filterShader = VehicleFilterShader; // TODO Make our own filter shader
	sceneDesc.simulationEventCallback = &gCollisionCallback; // Need to put this before createScene()
	this->mScene = mPhysics->createScene(sceneDesc);

	// Material
	this->mMaterial = mPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	// Cooking
	this->mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *mFoundation, PxCookingParams(PxTolerancesScale()));

	// Transmit scene data to PVD
	PxPvdSceneClient* pvdClient = mScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	//Create the batched scene queries for the suspension raycasts.
	this->mVehicleSceneQueryData = VehicleSceneQueryData::allocate(1, PX_MAX_NB_WHEELS, 1, 1, WheelSceneQueryPreFilterBlocking, NULL, this->mDefaultAllocatorCallback);
	this->mBatchQuery = VehicleSceneQueryData::setUpBatchedSceneQuery(0, *this->mVehicleSceneQueryData, mScene);

	//Create the friction table for each combination of tire and surface type.
	this->mFrictionPairs = createFrictionPairs(mMaterial);

	//Set all of the ground/player/object actors
	initializeActors();

	//Set the vehicle to rest in first gear.
	//Set the vehicle to use auto-gears.
	mVehiclePlayer1->setToRestState();
	mVehiclePlayer1->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
	mVehiclePlayer1->mDriveDynData.setUseAutoGears(true);

	//mVehicleModeTimer = 0.0f;
	//mVehicleOrderProgress = 0;
	startBrakeMode();
}

// See: https://gameworksdocs.nvidia.com/PhysX/4.1/documentation/physxguide/Manual/Vehicles.html
void PhysicsSystem::initVehicleSDK()
{
	// Initialize vehicle SDK
	physx::PxInitVehicleSDK(*mPhysics);

	// Set basis vectors
	physx::PxVehicleSetBasisVectors(physx::PxVec3(0, 1, 0), physx::PxVec3(0, 0, -1));

	// Set the vehicle update mode to be immediate velocity changes
	physx::PxVehicleSetUpdateMode(physx::PxVehicleUpdateMode::eVELOCITY_CHANGE);
}


void PhysicsSystem::update(const float timestep)
{
	// Update scene in physics simulation
	this->mScene->simulate(timestep);
	this->mScene->fetchResults(true);

	// Update the control inputs for the vehicle
	if (this->mMimicKeyInputs)
	{
		PxVehicleDrive4WSmoothDigitalRawInputsAndSetAnalogInputs(gKeySmoothingData, gSteerVsForwardSpeedTable, this->mVehicleInputData, timestep, this->mIsVehicleInAir, *this->mVehiclePlayer1);
	}
	else
	{
		PxVehicleDrive4WSmoothAnalogRawInputsAndSetAnalogInputs(gPadSmoothingData, gSteerVsForwardSpeedTable, this->mVehicleInputData, timestep, this->mIsVehicleInAir, *this->mVehiclePlayer1);
	}

	//Raycasts.
	PxVehicleWheels* vehicles[1] = { this->mVehiclePlayer1 };
	PxRaycastQueryResult* raycastResults = this->mVehicleSceneQueryData->getRaycastQueryResultBuffer(0);
	const PxU32 raycastResultsSize = this->mVehicleSceneQueryData->getQueryResultBufferSize();
	PxVehicleSuspensionRaycasts(this->mBatchQuery, 1, vehicles, raycastResultsSize, raycastResults);

	// Vehicle update
	const PxVec3 grav = this->mScene->getGravity();
	PxWheelQueryResult wheelQueryResults[PX_MAX_NB_WHEELS];
	PxVehicleWheelQueryResult vehicleQueryResults[1] = { {wheelQueryResults, this->mVehiclePlayer1->mWheelsSimData.getNbWheels()} };
	PxVehicleUpdates(timestep, grav, *this->mFrictionPairs, 1, vehicles, vehicleQueryResults);

	//Work out if the vehicle is in the air.
	this->mIsVehicleInAir = this->mVehiclePlayer1->getRigidDynamicActor()->isSleeping() ? false : PxVehicleIsInAir(vehicleQueryResults[0]);

	// Set player 1 transform
	Entity* player1 = g_scene.getEntity("player1");
	Transform* player1Transform = player1->getTransform();
	player1Transform->update(this->mVehiclePlayer1->getRigidDynamicActor()->getGlobalPose());

	// Update the food transforms
	updateFoodTransforms();

	// Set the ground's transform
	Entity* countertop = g_scene.getEntity("countertop");
	Transform* countertopTransform = countertop->getTransform();
	physx::PxShape* shapes[1];
	mGroundPlane->getShapes(shapes, 1);
	physx::PxTransform groundTransform = shapes[0]->getLocalPose();
	groundTransform.p.y -= 5; // lower the ground to not clip through the surface? slightly??
	countertopTransform->update(groundTransform);

	Transform* demoObstacle = g_scene.getEntity("demoObstacle")->getTransform();
	demoObstacle->update(this->demoObstacle->getGlobalPose());
}

void PhysicsSystem::updateFoodTransforms(){
	// Cheese
	Transform* cheese = g_scene.getEntity("cheese")->getTransform();
	cheese->update(this->cheese->getGlobalPose());

	// Sausagea
	Transform* sausage = g_scene.getEntity("sausage")->getTransform();
	sausage->update(this->sausage->getGlobalPose());

	// Tomato
	Transform* tomato = g_scene.getEntity("tomato")->getTransform();
	tomato->update(this->tomato->getGlobalPose());

	// Dough
	Transform* dough = g_scene.getEntity("dough")->getTransform();
	dough->update(this->dough->getGlobalPose());
}

void PhysicsSystem::cleanupPhysics()
{
	// Players
	this->mVehiclePlayer1->getRigidDynamicActor()->release();
	this->mVehiclePlayer1->free();

	// Ingredients
	this->cheese->release();
	this->tomato->release();
	this->dough->release();
	this->sausage->release();

	// PHYSX
	PX_RELEASE(this->mGroundPlane);
	PX_RELEASE(this->mBatchQuery);
	this->mVehicleSceneQueryData->free(this->mDefaultAllocatorCallback);
	PX_RELEASE(this->mFrictionPairs);
	PxCloseVehicleSDK();

	PX_RELEASE(this->mMaterial);
	PX_RELEASE(this->mCooking);
	PX_RELEASE(this->mScene);
	PX_RELEASE(this->mDispatcher);
	PX_RELEASE(this->mPhysics);
	if (this->mPvd)
	{
		PxPvdTransport* transport = this->mPvd->getTransport();
		this->mPvd->release();	this->mPvd = NULL;
		PX_RELEASE(transport);
	}
	PX_RELEASE(this->mFoundation);
}


PxRigidDynamic* PhysicsSystem::createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity = PxVec3(0))
{
	PxRigidDynamic* dynamic = PxCreateDynamic(*this->mPhysics, t, geometry, *this->mMaterial, 10.0f);
	dynamic->setAngularDamping(0.5f);
	dynamic->setLinearVelocity(velocity);
	this->mScene->addActor(*dynamic);
	return dynamic;
}


void PhysicsSystem::keyPress(unsigned char key)
{
	if (this->buttonState.forwardsHeld) {
		if (this->mVehiclePlayer1->mDriveDynData.mCurrentGear == snippetvehicle::PxVehicleGearsData::eNEUTRAL) {
			this->mVehiclePlayer1->mDriveDynData.forceGearChange(snippetvehicle::PxVehicleGearsData::eFIRST);
		}
		if (this->mVehiclePlayer1->mDriveDynData.mCurrentGear == snippetvehicle::PxVehicleGearsData::eFIRST) {
			this->mVehiclePlayer1->mDriveDynData.forceGearChange(snippetvehicle::PxVehicleGearsData::eSECOND);
		}
		if (this->mVehiclePlayer1->mDriveDynData.mCurrentGear == snippetvehicle::PxVehicleGearsData::eSECOND) {
			this->mVehiclePlayer1->mDriveDynData.forceGearChange(snippetvehicle::PxVehicleGearsData::eTHIRD);
		}
		this->mVehicleInputData.setDigitalBrake(false);
		this->mVehicleInputData.setAnalogBrake(0.f);
		startAccelerateForwardsMode();
	}
	if (this->buttonState.backwardsHeld) {
		//if (this->vehiclePlayer1->mDriveDynData.mCurrentGear != snippetvehicle::PxVehicleGearsData::eREVERSE)
		this->mVehiclePlayer1->mDriveDynData.forceGearChange(snippetvehicle::PxVehicleGearsData::eREVERSE);
		this->mVehicleInputData.setDigitalBrake(false);
		this->mVehicleInputData.setAnalogBrake(0.f);
		startAccelerateReverseMode();
	}
	if (this->buttonState.rightHeld) {
		startTurnHardRightMode();
	}
	if (this->buttonState.leftHeld) {
		startTurnHardLeftMode();
	}
	if (this->buttonState.brakeHeld) {
		startBrakeMode();
	}
	if (!this->buttonState.forwardsHeld && !this->buttonState.backwardsHeld && !this->buttonState.leftHeld &&
		!this->buttonState.rightHeld && !this->buttonState.brakeHeld) {
		//if (this->vehiclePlayer1->mDriveDynData.mCurrentGear != snippetvehicle::PxVehicleGearsData::eNEUTRAL)
		this->mVehiclePlayer1->mDriveDynData.forceGearChange(snippetvehicle::PxVehicleGearsData::eNEUTRAL);
		this->mVehicleInputData.setDigitalBrake(false);
		this->mVehicleInputData.setAnalogBrake(0.f);
		releaseAllControls();
	}
}

void PhysicsSystem::keyRelease(unsigned char key)
{
	PxTransform camera = PxTransform(PxVec3(1.0f));


	if (!this->buttonState.forwardsHeld) {
		releaseGas();
	}
	if (!this->buttonState.backwardsHeld) {
		releaseGas();
	}
	if (!this->buttonState.rightHeld) {
		releaseSteering();
	}
	if (!this->buttonState.leftHeld) {
		releaseSteering();
	}
	if (this->buttonState.brakeHeld) {
		startBrakeMode();
	}
	if (this->mVehiclePlayer1->mDriveDynData.mCurrentGear != snippetvehicle::PxVehicleGearsData::eNEUTRAL)
		releaseAllControls();
}

void PhysicsSystem::startAccelerateForwardsMode()
{
	if (this->mMimicKeyInputs)
	{
		this->mVehicleInputData.setDigitalAccel(true);
	}
	else
	{
		this->mVehicleInputData.setAnalogAccel(1.0f);
	}
}

void PhysicsSystem::startAccelerateReverseMode()
{
	//this->vehiclePlayer1->mDriveDynData.forceGearChange(PxVehicleGearsData::eREVERSE);

	if (this->mMimicKeyInputs)
	{
		this->mVehicleInputData.setDigitalAccel(true);
	}
	else
	{
		this->mVehicleInputData.setAnalogAccel(1.0f);
	}
}

void PhysicsSystem::startBrakeMode()
{
	if (this->mMimicKeyInputs)
	{
		this->mVehicleInputData.setDigitalBrake(true);
	}
	else
	{
		this->mVehicleInputData.setAnalogBrake(1.0f);
	}
}

void PhysicsSystem::startTurnHardLeftMode()
{
	if (this->mMimicKeyInputs)
	{
		this->mVehicleInputData.setDigitalAccel(true);
		this->mVehicleInputData.setDigitalSteerLeft(true);
	}
	else
	{
		//this->mVehicleInputData.setAnalogAccel(true);
		this->mVehicleInputData.setAnalogSteer(-1.0f);
	}
}

void PhysicsSystem::startTurnHardRightMode()
{
	if (this->mMimicKeyInputs)
	{
		this->mVehicleInputData.setDigitalAccel(true);
		this->mVehicleInputData.setDigitalSteerRight(true);
	}
	else
	{
		//this->mVehicleInputData.setAnalogAccel(1.0f);
		this->mVehicleInputData.setAnalogSteer(1.0f);
	}
}

void PhysicsSystem::startTurnSoftLeftMode(float value)
{
	if (this->mMimicKeyInputs)
	{
		this->mVehicleInputData.setDigitalAccel(true);
		this->mVehicleInputData.setDigitalSteerLeft(true);
	}
	else
	{
		//this->mVehicleInputData.setAnalogAccel(true);
		this->mVehicleInputData.setAnalogSteer(value);
	}
}

void PhysicsSystem::startTurnSoftRightMode(float value)
{
	if (this->mMimicKeyInputs)
	{
		this->mVehicleInputData.setDigitalAccel(true);
		this->mVehicleInputData.setDigitalSteerRight(true);
	}
	else
	{
		//this->mVehicleInputData.setAnalogAccel(1.0f);
		this->mVehicleInputData.setAnalogSteer(value);
	}
}

void PhysicsSystem::startHandbrakeTurnLeftMode()
{
	if (this->mMimicKeyInputs)
	{
		this->mVehicleInputData.setDigitalSteerLeft(true);
		this->mVehicleInputData.setDigitalHandbrake(true);
	}
	else
	{
		this->mVehicleInputData.setAnalogSteer(-1.0f);
		this->mVehicleInputData.setAnalogHandbrake(1.0f);
	}
}

void PhysicsSystem::startHandbrakeTurnRightMode()
{
	if (this->mMimicKeyInputs)
	{
		this->mVehicleInputData.setDigitalSteerRight(true);
		this->mVehicleInputData.setDigitalHandbrake(true);
	}
	else
	{
		this->mVehicleInputData.setAnalogSteer(1.0f);
		this->mVehicleInputData.setAnalogHandbrake(1.0f);
	}
}

void PhysicsSystem::releaseGas()
{
	if (this->mMimicKeyInputs)
	{
		this->mVehicleInputData.setDigitalAccel(false);
	}
	else
	{
		this->mVehicleInputData.setAnalogAccel(0.0f);
	}
}

void PhysicsSystem::releaseSteering()
{
	if (!this->mMimicKeyInputs)
	{
		this->mVehicleInputData.setAnalogSteer(0.0f);
	}
}


void PhysicsSystem::releaseAllControls()
{
	if (this->mMimicKeyInputs)
	{
		this->mVehicleInputData.setDigitalAccel(false);
		this->mVehicleInputData.setDigitalSteerLeft(false);
		this->mVehicleInputData.setDigitalSteerRight(false);
		this->mVehicleInputData.setDigitalBrake(false);
		this->mVehicleInputData.setDigitalHandbrake(false);
	}
	else
	{
		this->mVehicleInputData.setAnalogAccel(0.0f);
		this->mVehicleInputData.setAnalogSteer(0.0f);
		this->mVehicleInputData.setAnalogBrake(0.0f);
		this->mVehicleInputData.setAnalogHandbrake(0.0f);
	}
}
