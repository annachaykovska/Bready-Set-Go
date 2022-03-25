#include <cassert>

#include <glm/glm.hpp>

#include <snippetvehiclecommon/SnippetVehicleCreate.h>
#include <snippetvehiclecommon/SnippetVehicleFilterShader.h>
#include <snippetvehiclecommon/SnippetVehicleTireFriction.h>
#include <snippetutils/SnippetUtils.h>

#include "../SystemManager.h"
#include "../Rendering/RenderingSystem.h"
#include "PhysicsSystem.h"
#include "CollisionCallback.h"
#include "../Scene/Scene.h"
#include "../Transform.h"
#include "../Scene/Entity.h"
#include <Windows.h>

using namespace snippetvehicle;
using namespace physx;

extern Scene g_scene;
extern SystemManager g_systems;

CollisionCallback gCollisionCallback;

PxF32 gSteerVsForwardSpeedData[2 * 8] =
{
	0.0f,		0.2f,
	5.0f,		0.6f,
	30.0f,		0.8f,
	120.0f,		1.f,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32
};
PxFixedSizeLookupTable<8> gSteerVsForwardSpeedTable(gSteerVsForwardSpeedData, 4);

PxVehicleKeySmoothingData gKeySmoothingData =
{
	{
		10.0f,	//rise rate eANALOG_INPUT_ACCEL
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
	const PxF32 chassisMass = g_systems.physics->chassis_mass;
	const PxVec3 chassisDims(2.5f, 2.0f, 5.0f);
	const PxVec3 chassisMOI
	((chassisDims.y * chassisDims.y + chassisDims.z * chassisDims.z) * chassisMass / 12.0f,
		(chassisDims.x * chassisDims.x + chassisDims.z * chassisDims.z) * 0.8f * chassisMass / 12.0f,
		(chassisDims.x * chassisDims.x + chassisDims.y * chassisDims.y) * chassisMass / 12.0f);
	const PxVec3 chassisCMOffset(0.0f, -chassisDims.y * 0.5f + 0.65f, 0.25f);

	//Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
	//Moment of inertia is just the moment of inertia of a cylinder.
	const PxF32 wheelMass = g_systems.physics->wheel_mass;		//800.f
	const PxF32 wheelRadius = 0.5f;
	const PxF32 wheelWidth = 0.6f;
	const PxF32 wheelMOI = g_systems.physics->wheel_moi;		//2.f * wheelMass * wheelRadius * wheelRadius;
	const PxU32 nbWheels = 4;

	VehicleDesc vehicleDesc;

	vehicleDesc.chassisMass = chassisMass;
	vehicleDesc.chassisDims = chassisDims;
	vehicleDesc.chassisMOI = chassisMOI;
	vehicleDesc.chassisMOI.y *= g_systems.physics->chassis_moi_y;	//0.4
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

	PxTransform localTm(PxVec3(10, 2, 10));
	PxRigidDynamic* body = mPhysics->createRigidDynamic(t.transform(localTm));
	body->attachShape(*shape);
	PxRigidBodyExt::updateMassAndInertia(*body, 0.1f);

	// Set physx actor name
	body->setName(name.c_str());

	// Attach the entity to the physx actor
	void* vp = static_cast<void*>(new std::string(name));
	body->userData = vp;

	mScene->addActor(*body);
	shape->release();

	return body;
}

/*
PxRigidDynamic* PhysicsSystem::createObstacle(const PxTransform& t, PxReal halfExtent, std::string name)
{
	PxShape* shape = mPhysics->createShape(PxBoxGeometry(halfExtent, halfExtent, halfExtent), *mMaterial);
	PxFilterData cheeseFilter(COLLISION_FLAG_FOOD, COLLISION_FLAG_FOOD_AGAINST, 0, 0);
	shape->setSimulationFilterData(cheeseFilter);

	PxTransform localTm(PxVec3(0, 2, 0));
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
*/

void PhysicsSystem::updateCar() {
	PxVehicleDrive4W* cars[4];
	cars[0] = mVehiclePlayer1;
	cars[1] = mVehiclePlayer2;
	cars[2] = mVehiclePlayer3;
	cars[3] = mVehiclePlayer4;
	for (int car = 0; car < 4; car++) {
		// Update wheels
		for (int wheel = 0; wheel < 4; wheel++) {
			PxVehicleWheelData wheelData = cars[car]->mWheelsSimData.getWheelData(wheel);
			wheelData.mMass = wheel_mass;
			wheelData.mMOI = wheel_moi;
			wheelData.mMaxBrakeTorque = max_brake_torque;
			wheelData.mMaxHandBrakeTorque = max_hand_brake_torque;
			wheelData.mToeAngle = 0.f;
			wheelData.mDampingRate = wheel_damping_rate;
			if (wheel == PxVehicleDrive4WWheelOrder::eREAR_LEFT || wheel == PxVehicleDrive4WWheelOrder::eREAR_RIGHT) { // Rear wheel changes
				
			}
			else { // Front wheel changes

			}
			cars[car]->mWheelsSimData.setWheelData(wheel, wheelData);
		}

		// Update engine
		PxVehicleEngineData engineData = cars[car]->mDriveSimData.getEngineData();
		engineData.mPeakTorque = peak_torque;
		engineData.mMaxOmega = max_omega;
		//q1
		cars[car]->mDriveSimData.setEngineData(engineData);

		// Update chassis
		//PxVehicleChassisData gaming = cars[car]->
	}
}

void PhysicsSystem::updateEngine() {
	PxVehicleEngineData engine;
	engine.mPeakTorque = peak_torque;
	engine.mMaxOmega = max_omega;//approx 6000 rpm

	mVehiclePlayer1->mDriveSimData.setEngineData(engine);
	mVehiclePlayer2->mDriveSimData.setEngineData(engine);
	mVehiclePlayer3->mDriveSimData.setEngineData(engine);
	mVehiclePlayer4->mDriveSimData.setEngineData(engine);
}

void PhysicsSystem::initializeActors() {
	// KITCHEN ENVIRONMENT ---------------------------------------------------------------------------------------------------------
	// Create shape using the kitchen's mesh
	PxTriangleMeshGeometry kitchenGeometry = PxTriangleMeshGeometry(kitchenMesh, PxMeshScale());

	PxShape* kitchenShape = mPhysics->createShape(kitchenGeometry, *mMaterial, true);

	// Add filter data for ground
	PxFilterData kitchenFilter(COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST, 0, 0);
	kitchenShape->setSimulationFilterData(kitchenFilter);

	// Create the kitchen actor (rigid static)
	PxTransform kitchenTransform(PxVec3(0, -78, 50), PxQuat(PxIdentity));
	this->kitchen = mPhysics->createRigidStatic(kitchenTransform);
	this->kitchen->attachShape(*kitchenShape);

	// Set physx actor name
	this->kitchen->setName("countertop");

	// Attach the entity to the physx actor
	void* kitchenVp = static_cast<void*>(new std::string("countertop"));
	this->kitchen->userData = kitchenVp;

	// Get the kitchen shape so we can set query and simulation filter data
	PxShape* shapes[1];
	this->kitchen->getShapes(shapes, 1);

	//Set the query filter data of the kitchen so that the vehicle raycasts can hit the ground.
	PxFilterData qryFilterData;
	setupDrivableSurface(qryFilterData);
	shapes[0]->setQueryFilterData(qryFilterData);

	//Set the simulation filter data of the kitchen so that it collides with the chassis of a vehicle but not the wheels.
	shapes[0]->setSimulationFilterData(kitchenFilter);

	// Add the kitchen to the scene
	mScene->addActor(*(this->kitchen));

	// PLAYERS ---------------------------------------------------------------------------------------------------------------------
	//Create a vehicle that will drive on the plane.
	VehicleDesc vehicleDesc = initVehicleDesc(this->mMaterial);
	mVehiclePlayer1 = createVehicle4W(vehicleDesc, mPhysics, mCooking);
	mVehiclePlayer2 = createVehicle4W(vehicleDesc, mPhysics, mCooking);
	mVehiclePlayer3 = createVehicle4W(vehicleDesc, mPhysics, mCooking);
	mVehiclePlayer4 = createVehicle4W(vehicleDesc, mPhysics, mCooking);

	updateEngine();

	startTransformPlayer1 = PxTransform(PxVec3(10, (vehicleDesc.chassisDims.y * 0.5f + vehicleDesc.wheelRadius + 1.0f), 20), PxQuat(PxIdentity));
	startTransformPlayer2 = PxTransform(PxVec3(30, (vehicleDesc.chassisDims.y * 0.5f + vehicleDesc.wheelRadius + 1.0f), -20), PxQuat(PxIdentity));
	startTransformPlayer3 = PxTransform(PxVec3(-20, (vehicleDesc.chassisDims.y * 0.5f + vehicleDesc.wheelRadius + 1.0f), -10), PxQuat(PxIdentity));
	startTransformPlayer4 = PxTransform(PxVec3(-20, (vehicleDesc.chassisDims.y * 0.5f + vehicleDesc.wheelRadius + 1.0f), 20), PxQuat(PxIdentity));
	mVehiclePlayer1->getRigidDynamicActor()->setGlobalPose(startTransformPlayer1);
	mVehiclePlayer2->getRigidDynamicActor()->setGlobalPose(startTransformPlayer2);
	mVehiclePlayer3->getRigidDynamicActor()->setGlobalPose(startTransformPlayer3);
	mVehiclePlayer4->getRigidDynamicActor()->setGlobalPose(startTransformPlayer4);

	// Set physx actor name
	mVehiclePlayer1->getRigidDynamicActor()->setName("player1");
	mVehiclePlayer2->getRigidDynamicActor()->setName("player2");
	mVehiclePlayer3->getRigidDynamicActor()->setName("player3");
	mVehiclePlayer4->getRigidDynamicActor()->setName("player4");

	// Attach the entity to the physx actor
	Entity* player1 = g_scene.getEntity("player1");
	Entity* player2 = g_scene.getEntity("player2");
	Entity* player3 = g_scene.getEntity("player3");
	Entity* player4 = g_scene.getEntity("player4");

	void* vp1 = static_cast<void*>(new std::string("player1"));
	void* vp2 = static_cast<void*>(new std::string("player2"));
	void* vp3 = static_cast<void*>(new std::string("player3"));
	void* vp4 = static_cast<void*>(new std::string("player4"));

	mVehiclePlayer1->getRigidDynamicActor()->userData = vp1;
	mVehiclePlayer2->getRigidDynamicActor()->userData = vp2;
	mVehiclePlayer3->getRigidDynamicActor()->userData = vp3;
	mVehiclePlayer4->getRigidDynamicActor()->userData = vp4;

	PxShape* shapes1[1];
	PxShape* shapes2[1];
	PxShape* shapes3[1];
	PxShape* shapes4[1];
	this->mVehiclePlayer1->getRigidDynamicActor()->getShapes(shapes1, 1);
	this->mVehiclePlayer2->getRigidDynamicActor()->getShapes(shapes2, 1);
	this->mVehiclePlayer3->getRigidDynamicActor()->getShapes(shapes3, 1);
	this->mVehiclePlayer4->getRigidDynamicActor()->getShapes(shapes4, 1);
	PxFilterData qryVehicle1FilterData;
	PxFilterData qryVehicle2FilterData;
	PxFilterData qryVehicle3FilterData;
	PxFilterData qryVehicle4FilterData;
	qryVehicle1FilterData.word0 = PLAYERGROUP1;
	qryVehicle2FilterData.word0 = PLAYERGROUP2;
	qryVehicle3FilterData.word0 = PLAYERGROUP3;
	qryVehicle4FilterData.word0 = PLAYERGROUP4;
	shapes1[0]->setQueryFilterData(qryVehicle1FilterData);
	shapes2[0]->setQueryFilterData(qryVehicle2FilterData);
	shapes3[0]->setQueryFilterData(qryVehicle3FilterData);
	shapes4[0]->setQueryFilterData(qryVehicle4FilterData);

	mScene->addActor(*mVehiclePlayer1->getRigidDynamicActor());
	mScene->addActor(*mVehiclePlayer2->getRigidDynamicActor());
	mScene->addActor(*mVehiclePlayer3->getRigidDynamicActor());
	mScene->addActor(*mVehiclePlayer4->getRigidDynamicActor());

	// FOOD ITEMS ---------------------------------------------------------------------------------------------------------------------
	// Note: Physx actor name must match Entity name
	
	float halfExtent = 1.0f;

	// CHEESE
	PxTransform cheeseTransform(PxVec3(-220, 3, -130));
	this->cheese = createFoodBlock(cheeseTransform, halfExtent, "cheese");

	// SAUSAGE
	PxTransform sausageTransform(PxVec3(-110, 3, 175));
	this->sausage = createFoodBlock(sausageTransform, halfExtent, "sausage");

	// TOMATO
	PxTransform tomatoTransform(PxVec3(95, 3, 170));
	this->tomato = createFoodBlock(tomatoTransform, halfExtent, "tomato");

	// DOUGH
	PxTransform doughTransform(PxVec3(-105, 3, -8));
	this->dough = createFoodBlock(doughTransform, halfExtent, "dough");

	// CARROT
	PxTransform carrotTransform(PxVec3(-5, 3, 0));
	this->carrot = createFoodBlock(carrotTransform, halfExtent, "carrot");

	// LETTUCE
	PxTransform lettuceTransform(PxVec3(-10, 3, 0));
	this->lettuce = createFoodBlock(lettuceTransform, halfExtent, "lettuce");

	// PARSNIP
	PxTransform parsnipTransform(PxVec3(-15, 3, 0));
	this->parsnip = createFoodBlock(parsnipTransform, halfExtent, "parsnip");

	// RICE
	PxTransform riceTransform(PxVec3(-20, 3, 0));
	this->rice = createFoodBlock(riceTransform, halfExtent, "rice");

	// EGG
	PxTransform eggTransform(PxVec3(-25, 3, 0));
	this->egg = createFoodBlock(eggTransform, halfExtent, "egg");

	// CHICKEN
	PxTransform chickenTransform(PxVec3(-30, 3, 0));
	this->chicken = createFoodBlock(chickenTransform, halfExtent, "chicken");

	// PEAS
	PxTransform peasTransform(PxVec3(-35, 3, 0));
	this->peas = createFoodBlock(peasTransform, halfExtent, "peas");

	// SOUPBASE
	PxTransform soupbaseTransform(PxVec3(-40, 3, 0));
	this->soupbase = createFoodBlock(soupbaseTransform, halfExtent, "soupbase");

	// PUMPKIN
	PxTransform pumpkinTransform(PxVec3(-45, 3, 0));
	this->pumpkin = createFoodBlock(pumpkinTransform, halfExtent, "pumpkin");
}

PhysicsSystem::PhysicsSystem() :
		chassis_mass(400.f)
	,	peak_torque(100000.f)
	,	max_omega(1000.f)
	,	wheel_mass(800.f)
	,	wheel_damping_rate(20.f)
	,	wheel_moi(100.f)//wheel_moi(1440.f)
	,	chassis_moi_y(0.25f)
	,	max_brake_torque(100000.f)
	,	max_hand_brake_torque(50000.f)
{
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
	sceneDesc.gravity = PxVec3(0.0f, -30.f, 0.0f);
	sceneDesc.cpuDispatcher = this->mDispatcher;
	sceneDesc.filterShader = VehicleFilterShader; // TODO Make our own filter shader?
	sceneDesc.simulationEventCallback = &gCollisionCallback; // Need to put this before createScene()
	this->mScene = mPhysics->createScene(sceneDesc);

	// Material
	this->mMaterial = mPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	// Cooking
	this->mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *mFoundation, PxCookingParams(PxTolerancesScale()));
	if (!this->mCooking) std::cout << "PxCreateCooking failed!\n";

	// Triangle Mesh data initialization for cooking
	this->kitchenVerts = new std::vector<physx::PxVec3>();
	this->kitchenIndices = new std::vector<physx::PxU32>;
	kitchenVerts->reserve(40000);
	kitchenIndices->reserve(40000);

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

	// Initialize time tracking for updates
	this->mAccumulator = 0;
}

void PhysicsSystem::initialize()
{
	// Create the kitchen environment collision mesh
	cookKitchen();

	//Set all of the ground/player/object actors
	initializeActors();

	//Set the vehicle to rest in first gear.
	//Set the vehicle to use auto-gears.
	mVehiclePlayer1->setToRestState();
	mVehiclePlayer2->setToRestState();
	mVehiclePlayer3->setToRestState();
	mVehiclePlayer4->setToRestState();
	mVehiclePlayer1->mDriveDynData.setUseAutoGears(true);
	mVehiclePlayer2->mDriveDynData.setUseAutoGears(true);
	mVehiclePlayer3->mDriveDynData.setUseAutoGears(true);
	mVehiclePlayer4->mDriveDynData.setUseAutoGears(true);
	mVehiclePlayer1->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
	mVehiclePlayer2->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
	mVehiclePlayer3->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
	mVehiclePlayer4->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
	//mVehicleInputData.setAnalogBrake(1.0f);

	viewDirectionalInfluence = 0.f;
	turnDirectionalInfluence = 0.f;

	updateCar(); // TODO THIS IS EXTREMELY SCUFFED LOOK FOR A BETTER WAY TO DO THIS
}

// Converts the assimp importer mesh in the rendering system to a format that physx
// can use to produce a collider mesh
void PhysicsSystem::cookKitchen()
{
	Model* kitchenModel = g_systems.render->getKitchenModel();

	kitchenModel->physicsVerts(kitchenVerts);
	kitchenModel->physicsIndices(kitchenIndices);

	PxTriangleMeshDesc meshDesc;
	meshDesc.setToDefault();

	meshDesc.points.count = kitchenVerts->size(); // PxU32
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.points.data = kitchenVerts->data(); // PxVec3 []

	meshDesc.triangles.count = kitchenIndices->size() / 3; // PxU32
	meshDesc.triangles.stride = 3 * sizeof(PxU32);
	meshDesc.triangles.data = kitchenIndices->data(); // PxU32 []

	PxDefaultMemoryOutputStream writeBuffer;
	PxTriangleMeshCookingResult::Enum result;

	this->mCooking->cookTriangleMesh(meshDesc, writeBuffer, &result);

	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());

	this->kitchenMesh = mPhysics->createTriangleMesh(readBuffer);
}

// See: https://gameworksdocs.nvidia.com/PhysX/4.1/documentation/physxguide/Manual/Vehicles.html
void PhysicsSystem::initVehicleSDK()
{
	// Initialize vehicle SDK
	physx::PxInitVehicleSDK(*mPhysics);

	// Set basis vectors
	physx::PxVehicleSetBasisVectors(physx::PxVec3(0, 1.0f, 0), physx::PxVec3(0, 0, 1.0f));

	// Set the vehicle update mode to be immediate velocity changes
	physx::PxVehicleSetUpdateMode(physx::PxVehicleUpdateMode::eVELOCITY_CHANGE);
}

void PhysicsSystem::setAnalogInputs(bool input) {
	this->useAnalogInputs = input;
}

void PhysicsSystem::setViewDirectionalInfluence(float value)
{
	viewDirectionalInfluence = value;
}

float PhysicsSystem::getViewDirectionalInfluence()
{
	return viewDirectionalInfluence;
}

void PhysicsSystem::setTurnDirectionalInfluence(float value)
{
	turnDirectionalInfluence = value;
}

float PhysicsSystem::getTurnDirectionalInfluence()
{
	return turnDirectionalInfluence;
}

void PhysicsSystem::updateVehicle(PxVehicleDrive4W* player, bool& isVehicleInAir, PxVehicleDrive4WRawInputData& inputData, std::string entityName, const float timestep) {
	// Update the control inputs for the vehicle
	if (useAnalogInputs) {
		PxVehicleDrive4WSmoothAnalogRawInputsAndSetAnalogInputs(gPadSmoothingData, gSteerVsForwardSpeedTable, inputData, timestep, isVehicleInAir, *player);
	}
	else {
		PxVehicleDrive4WSmoothDigitalRawInputsAndSetAnalogInputs(gKeySmoothingData, gSteerVsForwardSpeedTable, inputData, timestep, isVehicleInAir, *player);
	}

	// Raycasts
	PxVehicleWheels* vehicles[1] = { player };
	PxRaycastQueryResult* raycastResults = this->mVehicleSceneQueryData->getRaycastQueryResultBuffer(0);
	const PxU32 raycastResultsSize = this->mVehicleSceneQueryData->getQueryResultBufferSize();
	PxVehicleSuspensionRaycasts(this->mBatchQuery, 1, vehicles, raycastResultsSize, raycastResults);

	// Check for collisions if recorded
	Entity* player1 = g_scene.getEntity("player1");
	Entity* player2 = g_scene.getEntity("player2");
	Entity* player3 = g_scene.getEntity("player3");
	Entity* player4 = g_scene.getEntity("player4");

	if (player1->verifyPlayerCollision) {
		if (player1->otherPlayerInCollision == "player2")
			playerCollisionRaycast(player1, mVehiclePlayer1, g_scene.getEntity(player1->otherPlayerInCollision), mVehiclePlayer2);
		else if (player1->otherPlayerInCollision == "player3")
			playerCollisionRaycast(player1, mVehiclePlayer1, g_scene.getEntity(player1->otherPlayerInCollision), mVehiclePlayer3);
		else if (player1->otherPlayerInCollision == "player4")
			playerCollisionRaycast(player1, mVehiclePlayer1, g_scene.getEntity(player1->otherPlayerInCollision), mVehiclePlayer4);
	}
	else if (player2->verifyPlayerCollision) {
		if (player2->otherPlayerInCollision == "player1")
			playerCollisionRaycast(player2, mVehiclePlayer2, g_scene.getEntity(player2->otherPlayerInCollision), mVehiclePlayer1);
		else if (player2->otherPlayerInCollision == "player3")
			playerCollisionRaycast(player2, mVehiclePlayer2, g_scene.getEntity(player2->otherPlayerInCollision), mVehiclePlayer3);
		else if (player2->otherPlayerInCollision == "player4")
			playerCollisionRaycast(player2, mVehiclePlayer2, g_scene.getEntity(player2->otherPlayerInCollision), mVehiclePlayer4);
	}
	else if (player3->verifyPlayerCollision) {
		if (player3->otherPlayerInCollision == "player1")
			playerCollisionRaycast(player3, mVehiclePlayer3, g_scene.getEntity(player3->otherPlayerInCollision), mVehiclePlayer1);
		else if (player3->otherPlayerInCollision == "player2")
			playerCollisionRaycast(player3, mVehiclePlayer3, g_scene.getEntity(player3->otherPlayerInCollision), mVehiclePlayer2);
		else if (player3->otherPlayerInCollision == "player4")
			playerCollisionRaycast(player3, mVehiclePlayer3, g_scene.getEntity(player3->otherPlayerInCollision), mVehiclePlayer4);
	}
	else if (player4->verifyPlayerCollision) {
		if (player4->otherPlayerInCollision == "player1")
			playerCollisionRaycast(player4, mVehiclePlayer4, g_scene.getEntity(player4->otherPlayerInCollision), mVehiclePlayer1);
		else if (player4->otherPlayerInCollision == "player2")
			playerCollisionRaycast(player4, mVehiclePlayer4, g_scene.getEntity(player4->otherPlayerInCollision), mVehiclePlayer2);
		else if (player4->otherPlayerInCollision == "player3")
			playerCollisionRaycast(player4, mVehiclePlayer4, g_scene.getEntity(player4->otherPlayerInCollision), mVehiclePlayer3);
	}

	// Vehicle update
	const PxVec3 grav = this->mScene->getGravity();
	PxWheelQueryResult wheelQueryResults[PX_MAX_NB_WHEELS];
	PxVehicleWheelQueryResult vehicleQueryResults[1] = { {wheelQueryResults, player->mWheelsSimData.getNbWheels()} };
	PxVehicleUpdates(timestep, grav, *this->mFrictionPairs, 1, vehicles, vehicleQueryResults);

	// Work out if the vehicle is in the air, add an opposing force if in air
	float horizontalFactor = -400.0f;
	float verticalFactor = -55.0f;
	if (entityName == "player1") {
		this->mIsVehicleInAirPlayer1 = player->getRigidDynamicActor()->isSleeping() ? false : PxVehicleIsInAir(vehicleQueryResults[0]);
		if (this->mIsVehicleInAirPlayer1 && !this->mImpulseAppliedPlayer1) {
			this->mImpulseAppliedPlayer1 = true;
			PxVec3 velocity = player->getRigidDynamicActor()->getLinearVelocity();
			player->getRigidDynamicActor()->setLinearVelocity(PxVec3(velocity.x, velocity.y / 4, velocity.z));
			player->getRigidDynamicActor()->addForce(PxVec3(0.0, verticalFactor, 0.0), PxForceMode::eIMPULSE, true);
		}
		else if (!this->mIsVehicleInAirPlayer1 && this->mImpulseAppliedPlayer1) {
			this->mImpulseAppliedPlayer1 = false;
		}
	}
	else if (entityName == "player2") {
		this->mIsVehicleInAirPlayer2 = player->getRigidDynamicActor()->isSleeping() ? false : PxVehicleIsInAir(vehicleQueryResults[0]);
		if (this->mIsVehicleInAirPlayer2 && !this->mImpulseAppliedPlayer2) {
			this->mImpulseAppliedPlayer2 = true;
			PxVec3 velocity = player->getRigidDynamicActor()->getLinearVelocity();
			player->getRigidDynamicActor()->setLinearVelocity(PxVec3(velocity.x, velocity.y / 4, velocity.z));
			player->getRigidDynamicActor()->addForce(PxVec3(0.0, verticalFactor, 0.0), PxForceMode::eIMPULSE, true);
		}
		else if (!this->mIsVehicleInAirPlayer2 && this->mImpulseAppliedPlayer2) {
			this->mImpulseAppliedPlayer2 = false;
		}
	}
	else if (entityName == "player3") {
		this->mIsVehicleInAirPlayer3 = player->getRigidDynamicActor()->isSleeping() ? false : PxVehicleIsInAir(vehicleQueryResults[0]);
		if (this->mIsVehicleInAirPlayer3 && !this->mImpulseAppliedPlayer3) {
			this->mImpulseAppliedPlayer3 = true;
			PxVec3 velocity = player->getRigidDynamicActor()->getLinearVelocity();
			player->getRigidDynamicActor()->setLinearVelocity(PxVec3(velocity.x, velocity.y / 4, velocity.z));
			player->getRigidDynamicActor()->addForce(PxVec3(0.0, verticalFactor, 0.0), PxForceMode::eIMPULSE, true);
		}
		else if (!this->mIsVehicleInAirPlayer3 && this->mImpulseAppliedPlayer3) {
			this->mImpulseAppliedPlayer3 = false;
		}
	}
	else if (entityName == "player4") {
		this->mIsVehicleInAirPlayer4 = player->getRigidDynamicActor()->isSleeping() ? false : PxVehicleIsInAir(vehicleQueryResults[0]);
		if (this->mIsVehicleInAirPlayer4 && !this->mImpulseAppliedPlayer4) {
			this->mImpulseAppliedPlayer4 = true;
			PxVec3 velocity = player->getRigidDynamicActor()->getLinearVelocity();
			player->getRigidDynamicActor()->setLinearVelocity(PxVec3(velocity.x, velocity.y / 4, velocity.z));
			player->getRigidDynamicActor()->addForce(PxVec3(0.0, verticalFactor, 0.0), PxForceMode::eIMPULSE, true);
		}
		else if (!this->mIsVehicleInAirPlayer4 && this->mImpulseAppliedPlayer4) {
			this->mImpulseAppliedPlayer4 = false;
		}
	}

	// Set the angular velocity
	PxVec3 ang_vel = player->getRigidDynamicActor()->getAngularVelocity();
	player->getRigidDynamicActor()->setAngularVelocity(PxVec3(ang_vel.x / 1.1f, ang_vel.y / 1.1f, ang_vel.z / 1.1f));

	// Set player 1 transform
	Entity* playerEntity = g_scene.getEntity(entityName);
	Transform* playerTransform = playerEntity->getTransform();
	playerTransform->update(player->getRigidDynamicActor()->getGlobalPose());
}

void PhysicsSystem::update(const float dt)
{
	// Only update if more than 1/120th of a second has passed since last update
	// Making the constant much larger than 1/120th causes a significant jitter
	float timestep = 1.0f / 120.0f;
	this->mAccumulator += dt;
	if (this->mAccumulator < timestep)
		return;

	this->mAccumulator -= timestep;

	PxTransform trans = PxTransform(PxVec3(0, -100, 0));
	PxTransform reset = PxTransform(PxVec3(0, 5.0, 0));
	if (g_scene.getEntity("tomato")->removeFlag)
		this->tomato->setGlobalPose(trans);
	if (g_scene.getEntity("cheese")->removeFlag)
		this->cheese->setGlobalPose(trans);
	if (g_scene.getEntity("sausage")->removeFlag)
		this->sausage->setGlobalPose(trans);
	if (g_scene.getEntity("dough")->removeFlag)
		this->dough->setGlobalPose(trans);
	if (this->mVehiclePlayer1->getRigidDynamicActor()->getGlobalPose().p.y < -100.0)
		this->mVehiclePlayer1->getRigidDynamicActor()->setGlobalPose(reset);

	trans = this->mVehiclePlayer1->getRigidDynamicActor()->getGlobalPose();
	g_scene.getEntity("player1")->speed = (float)mVehiclePlayer1->computeForwardSpeed();
	glm::quat newQuat = glm::quat(trans.q.w, trans.q.x, trans.q.y, trans.q.z);
	glm::mat4 rotationMat = toMat4(newQuat);
	g_scene.getEntity("player1")->orientation = rotationMat * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f);
	g_scene.getEntity("player2")->speed = (float)mVehiclePlayer2->computeForwardSpeed();
	g_scene.getEntity("player3")->speed = (float)mVehiclePlayer3->computeForwardSpeed();
	g_scene.getEntity("player4")->speed = (float)mVehiclePlayer4->computeForwardSpeed();

	mVehiclePlayer1->getRigidDynamicActor()->getGlobalPose().q;

	// Update scene in physics simulation
	this->mScene->simulate(timestep);
	this->mScene->fetchResults(true);

	// Update the players
	updateVehicle(this->mVehiclePlayer1, this->mIsVehicleInAirPlayer1, this->mVehicleInputDataPlayer1, "player1", timestep);
	updateVehicle(this->mVehiclePlayer2, this->mIsVehicleInAirPlayer2, this->mVehicleInputDataPlayer2, "player2", timestep);
	updateVehicle(this->mVehiclePlayer3, this->mIsVehicleInAirPlayer3, this->mVehicleInputDataPlayer3, "player3", timestep);
	updateVehicle(this->mVehiclePlayer4, this->mIsVehicleInAirPlayer4, this->mVehicleInputDataPlayer4, "player4", timestep);

	// Update the food transforms
	updateFoodTransforms();
}

void PhysicsSystem::updateFoodTransforms()
{
	// Cheese
	g_scene.getEntity("cheese")->getTransform()->update(this->cheese->getGlobalPose());

	// Sausage
	g_scene.getEntity("sausage")->getTransform()->update(this->sausage->getGlobalPose());

	// Tomato
	g_scene.getEntity("tomato")->getTransform()->update(this->tomato->getGlobalPose());

	// Dough
	g_scene.getEntity("dough")->getTransform()->update(this->dough->getGlobalPose());

	// Carrot
	g_scene.getEntity("carrot")->getTransform()->update(this->carrot->getGlobalPose());

	// Lettuce
	g_scene.getEntity("lettuce")->getTransform()->update(this->lettuce->getGlobalPose());

	// Parsnip
	g_scene.getEntity("parsnip")->getTransform()->update(this->parsnip->getGlobalPose());

	// Rice
	g_scene.getEntity("rice")->getTransform()->update(this->rice->getGlobalPose());

	// Egg
	g_scene.getEntity("egg")->getTransform()->update(this->egg->getGlobalPose());

	// Chicken
	g_scene.getEntity("chicken")->getTransform()->update(this->chicken->getGlobalPose());

	// Peas
	g_scene.getEntity("peas")->getTransform()->update(this->peas->getGlobalPose());

	// Soupbase
	g_scene.getEntity("soupbase")->getTransform()->update(this->soupbase->getGlobalPose());

	// Pumpkin
	g_scene.getEntity("pumpkin")->getTransform()->update(this->pumpkin->getGlobalPose());
}

void PhysicsSystem::cleanupPhysics()
{
	// Triangle Mesh
	delete kitchenVerts;
	delete kitchenIndices;

	// Players
	this->mVehiclePlayer1->getRigidDynamicActor()->release();
	this->mVehiclePlayer2->getRigidDynamicActor()->release();
	this->mVehiclePlayer3->getRigidDynamicActor()->release();
	this->mVehiclePlayer4->getRigidDynamicActor()->release();
	this->mVehiclePlayer1->free();
	this->mVehiclePlayer2->free();
	this->mVehiclePlayer3->free();
	this->mVehiclePlayer4->free();

	// Ingredients
	this->cheese->release();
	this->tomato->release();
	this->dough->release();
	this->sausage->release();

	// PHYSX
	PX_RELEASE(this->mGroundPlane);
	PX_RELEASE(this->kitchen);
	PX_RELEASE(this->kitchenMesh);
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

float PhysicsSystem::getPlayerSpeed(int playerNumber)
{
	float result;

	switch (playerNumber)
	{
	case (1):
		result = (float)mVehiclePlayer1->computeForwardSpeed();
		return result;
		break;
	case (2):
		result = (float)mVehiclePlayer2->computeForwardSpeed();
		return result;
		break;
	case (3):
		result = (float)mVehiclePlayer3->computeForwardSpeed();
		return result;
		break;
	case (4):
		result = (float)mVehiclePlayer4->computeForwardSpeed();
		return result;
		break;
	default:
		break;
	}
}

void PhysicsSystem::respawnPlayer(int playerNumber) {
	switch (playerNumber)
	{
	case (1):
		mVehiclePlayer1->getRigidDynamicActor()->setGlobalPose(startTransformPlayer1);
		//mVehiclePlayer1->getRigidDynamicActor()->setLinearVelocity(PxVec3(0.0), false);
		//mVehiclePlayer1->getRigidDynamicActor()->setAngularVelocity(PxVec3(0.0), false);
		mVehiclePlayer1->setToRestState();
		break;
	case (2):
		mVehiclePlayer2->getRigidDynamicActor()->setGlobalPose(startTransformPlayer2);
		mVehiclePlayer2->setToRestState();
		break;
	case (3):
		mVehiclePlayer3->getRigidDynamicActor()->setGlobalPose(startTransformPlayer3);
		mVehiclePlayer3->setToRestState();
		break;
	case (4):
		mVehiclePlayer4->getRigidDynamicActor()->setGlobalPose(startTransformPlayer4);
		mVehiclePlayer4->setToRestState();
		break;
	default:
		break;
	}
}

void PhysicsSystem::playerCollisionRaycast(Entity* firstActor, PxVehicleDrive4W* firstVehicle, Entity* secondActor, PxVehicleDrive4W* secondVehicle) {
	// Variables
	const PxHitFlags outputFlags = PxHitFlag::eDEFAULT;
	const float cooldownThreshold = 3.0;
	PxReal maxDistance = 5;
	const PxU32 bufferSize = 256;
	int collisionWinner = 0;

	// Set rotation matrices
	float theta = glm::radians(25.f);
	glm::mat3 rotationMatrixLeft = glm::mat3(
		cos(theta), sin(theta), 0,
		-sin(theta), cos(theta), 0,
		0, 0, 1.0f);
	glm::mat3 rotationMatrixRight = glm::mat3(
		cos(-1 * theta), sin(-1 * theta), 0,
		-sin(-1 * theta), cos(-1 * theta), 0,
		0, 0, 1.0f);

	// FIRST ACTOR RAYCAST ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	int firstCollisionNum = 0;
	//printf("First Actor: %s\n", firstActor->name.c_str());
	PxVec3 origin = firstVehicle->getRigidDynamicActor()->getGlobalPose().p;
	PxVec3 unitDir = firstVehicle->getRigidDynamicActor()->getLinearVelocity().getNormalized();
	PxRaycastHit hitBuffer[bufferSize];
	PxRaycastBuffer buf(hitBuffer, bufferSize);
	PxQueryFilterData filterData1 = PxQueryFilterData();
	if (firstActor->name.c_str() == "player1")
		filterData1.data.word0 = PLAYERGROUP1;
	if (firstActor->name.c_str() == "player2")
		filterData1.data.word0 = PLAYERGROUP2;
	if (firstActor->name.c_str() == "player3")
		filterData1.data.word0 = PLAYERGROUP3;
	if (firstActor->name.c_str() == "player4")
		filterData1.data.word0 = PLAYERGROUP4;
	mScene->raycast(origin, unitDir, maxDistance, buf, outputFlags, filterData1);
	firstCollisionNum += buf.nbTouches;
	/*for (PxU32 i = 0; i < buf.nbTouches; i++)
		printf("Result ONE: %d\n", i);*/

	// Left raycast
	glm::vec3 newDir = rotationMatrixLeft * glm::vec3(unitDir.x, unitDir.y, unitDir.z);
	mScene->raycast(origin, PxVec3(newDir.x, newDir.y, newDir.z), maxDistance, buf, outputFlags, filterData1);
	firstCollisionNum += buf.nbTouches;

	// Right raycast
	newDir = rotationMatrixRight * glm::vec3(unitDir.x, unitDir.y, unitDir.z);
	mScene->raycast(origin, PxVec3(newDir.x, newDir.y, newDir.z), maxDistance, buf, outputFlags, filterData1);
	firstCollisionNum += buf.nbTouches;
	//printf("NUM COLLISION 1: %d\n", firstCollisionNum);


	// SECOND ACTOR RAYCAST ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	int secondCollisionNum = 0;
	//printf("Second Actor: %s\n", secondActor->name.c_str());
	origin = secondVehicle->getRigidDynamicActor()->getGlobalPose().p;
	unitDir = secondVehicle->getRigidDynamicActor()->getLinearVelocity().getNormalized();
	PxRaycastHit hitBuffer2[bufferSize];
	PxRaycastBuffer buf2(hitBuffer2, bufferSize);
	PxQueryFilterData filterData2 = PxQueryFilterData();
	if (secondActor->name.c_str() == "player1")
		filterData2.data.word0 = PLAYERGROUP1;
	if (secondActor->name.c_str() == "player2")
		filterData2.data.word0 = PLAYERGROUP2;
	if (secondActor->name.c_str() == "player3")
		filterData2.data.word0 = PLAYERGROUP3;
	if (secondActor->name.c_str() == "player4")
		filterData2.data.word0 = PLAYERGROUP4;
	mScene->raycast(origin, unitDir, maxDistance, buf2, outputFlags, filterData2);
	secondCollisionNum += buf2.nbTouches;
	/*for (PxU32 i = 0; i < buf2.nbTouches; i++)
		printf("Result TWO: %d\n", i);*/

	// Left raycast
	newDir = rotationMatrixLeft * glm::vec3(unitDir.x, unitDir.y, unitDir.z);
	mScene->raycast(origin, PxVec3(newDir.x, newDir.y, newDir.z), maxDistance, buf2, outputFlags, filterData1);
	secondCollisionNum += buf2.nbTouches;

	// Right raycast
	newDir = rotationMatrixRight * glm::vec3(unitDir.x, unitDir.y, unitDir.z);
	mScene->raycast(origin, PxVec3(newDir.x, newDir.y, newDir.z), maxDistance, buf2, outputFlags, filterData1);
	secondCollisionNum += buf2.nbTouches;
	//printf("NUM COLLISION 2: %d\n", secondCollisionNum);


	// DECIDE COLLISION WINNER AND CHECK COOLDOWN ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	float currentTime = glfwGetTime();
	bool cooldownIncomplete = false;
	if (buf.nbTouches > buf2.nbTouches || secondVehicle->getRigidDynamicActor()->getLinearVelocity() == PxVec3(0.0f)) {
		if (currentTime - firstActor->collisionCooldownStart > cooldownThreshold) {
			firstActor->collisionCooldownStart = -1.0;
			collisionWinner = 1;
		}
		else {
			cooldownIncomplete = true;
			printf("Cooldown for %s is not over yet.\n", firstActor->name.c_str());
		}
	}
	else if (buf.nbTouches < buf2.nbTouches || firstVehicle->getRigidDynamicActor()->getLinearVelocity() == PxVec3(0.0f)) {
		if (currentTime - secondActor->collisionCooldownStart > cooldownThreshold) {
			secondActor->collisionCooldownStart = -1.0;
			collisionWinner = 2;
		}
		else {
			cooldownIncomplete = true;
			printf("Cooldown for %s is not over yet.\n", secondActor->name.c_str());
		}
	}
	

	// RESOLVE COLLISION INGREDIENT EXCHANGE ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	if (collisionWinner == 0) {
		// No winners (headon collision)
		if (!cooldownIncomplete)
			printf("No collision winners. No ingredients moved.\n");
	}
	else {
		Inventory* player1Inventory = (Inventory*)firstActor->getComponent("inventory");
		Inventory* player2Inventory = (Inventory*)secondActor->getComponent("inventory");
		if (collisionWinner == 1) {
			int ingredient = player2Inventory->removeRandomPizzaIngredient(player1Inventory->cheese, player1Inventory->dough, player1Inventory->sausage, player1Inventory->tomato);
			player1Inventory->setIngredientFromId(ingredient);
			firstActor->collisionCooldownStart = glfwGetTime();
			printf("%s took food id %d from %s\n", firstActor->name.c_str(), ingredient, secondActor->name.c_str());
			
		}
		else if (collisionWinner == 2) {
			int ingredient = player1Inventory->removeRandomPizzaIngredient(player2Inventory->cheese, player2Inventory->dough, player2Inventory->sausage, player2Inventory->tomato);
			player2Inventory->setIngredientFromId(ingredient);
			secondActor->collisionCooldownStart = glfwGetTime();
			printf("%s took food id %d from %s\n\n", secondActor->name.c_str(), ingredient, firstActor->name.c_str());
			
		}
	}

	// RESET FLAGS
	firstActor->verifyPlayerCollision = false;
	secondActor->verifyPlayerCollision = false;
	firstActor->otherPlayerInCollision = "";
	secondActor->otherPlayerInCollision = "";
}