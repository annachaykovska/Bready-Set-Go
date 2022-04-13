#include <cassert>

#include <glm/glm.hpp>

#include <snippetvehiclecommon/SnippetVehicleCreate.h>
#include <snippetvehiclecommon/SnippetVehicleFilterShader.h>
#include <snippetvehiclecommon/SnippetVehicleTireFriction.h>
#include <snippetutils/SnippetUtils.h>

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "../SystemManager.h"
#include "../Rendering/RenderingSystem.h"
#include "PhysicsSystem.h"
#include "CollisionCallback.h"
#include "../Scene/Scene.h"
#include "../Scene/SpawnLocations.h"
#include "../Transform.h"
#include "../Scene/Entity.h"
#include <random>
#include <algorithm>
#include <Windows.h>
#include <algorithm>
#include <random>
#include "../Scene/SpawnLocations.h"
#include "../Gameplay/Recipe.h"

#include "../Audio/AudioSource.h"
#include "../Audio/AudioSystem.h"

#define PI 3.14159f

using namespace snippetvehicle;
using namespace physx;

namespace
{
	int spawnLocations[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
}

extern Scene g_scene;
extern SystemManager g_systems;

CollisionCallback gCollisionCallback;

PxF32 gSteerVsForwardSpeedData[2 * 10] =
{
	0.0f,		0.0f,
	10.0f,		0.2f,
	20.0f,		0.6f,
	30.0f,		1.0f,
	40.0f,		0.7f,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32,
};
PxFixedSizeLookupTable<8> gSteerVsForwardSpeedTable(gSteerVsForwardSpeedData, 4);

PxVehicleKeySmoothingData gKeySmoothingData =
{
	{
		10.0f,	//rise rate eANALOG_INPUT_ACCEL
		100.0f,	//rise rate eANALOG_INPUT_BRAKE		
		6.0f,	//rise rate eANALOG_INPUT_HANDBRAKE	
		2.5f,	//rise rate eANALOG_INPUT_STEER_LEFT
		2.5f,	//rise rate eANALOG_INPUT_STEER_RIGHT
	},
	{
		10.0f,	//fall rate eANALOG_INPUT_ACCEL
		100.0f,	//fall rate eANALOG_INPUT_BRAKE		
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

PxRigidDynamic* PhysicsSystem::createFoodBlock(const PxTransform& t, float x, float y, float z, std::string name)
{
	PxShape* shape = mPhysics->createShape(PxBoxGeometry(x / 2.0f, y / 2.0f, z / 2.0f), *mMaterial);
	PxFilterData cheeseFilter(COLLISION_FLAG_FOOD, COLLISION_FLAG_FOOD_AGAINST, 0, 0);
	shape->setSimulationFilterData(cheeseFilter);

	PxRigidDynamic* body = mPhysics->createRigidDynamic(t);
	body->attachShape(*shape);
	PxRigidBodyExt::updateMassAndInertia(*body, 0.1f);

	// Set physx actor name
	body->setName(name.c_str());

	// Attach the entity to the physx actor
	void* vp = static_cast<void*>(new std::string(name));
	body->userData = vp;

	// Add the collider to the scene
	mScene->addActor(*body);
	shape->release();

	return body;
}

void PhysicsSystem::updateCar() {
	PxVehicleDrive4W* cars[4];
	cars[0] = mVehiclePlayer1;
	cars[1] = mVehiclePlayer2;
	cars[2] = mVehiclePlayer3;
	cars[3] = mVehiclePlayer4;
	for (int car = 0; car < 4; car++) {
		// Update wheels/suspension
		for (int wheel = 0; wheel < 4; wheel++) {
			PxVehicleWheelData wheelData = cars[car]->mWheelsSimData.getWheelData(wheel);
			PxVehicleSuspensionData suspData = cars[car]->mWheelsSimData.getSuspensionData(wheel);

			// Wheels

			wheelData.mMass = wheel_mass;
			wheelData.mMOI = wheel_moi;
			wheelData.mMaxBrakeTorque = max_brake_torque;
			wheelData.mToeAngle = 0.f;
			wheelData.mDampingRate = wheel_damping_rate;
			
			// Suspension
			suspData.mSpringStrength = spring_strength;
			suspData.mMaxCompression = max_compression;
			suspData.mMaxDroop = max_droop;

			// Properties dependent on position
			if (wheel == PxVehicleDrive4WWheelOrder::eREAR_LEFT || wheel == PxVehicleDrive4WWheelOrder::eREAR_RIGHT) { // Rear changes
				wheelData.mMaxHandBrakeTorque = max_hand_brake_torque;
			}
			else { // Front changes
				wheelData.mMaxSteer = max_steer;
			}

			if (wheel == PxVehicleDrive4WWheelOrder::eREAR_LEFT || wheel == PxVehicleDrive4WWheelOrder::eFRONT_LEFT) { // Left changes
				suspData.mCamberAtRest = camber_at_rest;
			}
			else { // right changes
				suspData.mCamberAtRest = -camber_at_rest;
			}
			// Updating
			//cars[car]->mWheelsSimData.setSuspTravelDirection(wheel, PxVec3(0, -1, 0));
			cars[car]->mWheelsSimData.setSuspensionData(wheel, suspData);
			cars[car]->mWheelsSimData.setWheelData(wheel, wheelData);
		}

		// Update engine
		PxVehicleEngineData engineData = cars[car]->mDriveSimData.getEngineData();
		engineData.mPeakTorque = peak_torque;
		engineData.mMaxOmega = max_omega;
		//q1
		cars[car]->mDriveSimData.setEngineData(engineData);
	}
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

	glm::quat rotate = glm::angleAxis(glm::radians(180.0f), glm::vec3(0, 1.0f, 0));
	PxQuat initQuat = PxQuat(rotate.x, rotate.y, rotate.z, rotate.w);

	startTransformPlayer1 = PxTransform(PxVec3(33, (vehicleDesc.chassisDims.y * 0.5f + vehicleDesc.wheelRadius + 5.0f), 60), initQuat);
	startTransformPlayer2 = PxTransform(PxVec3(11, (vehicleDesc.chassisDims.y * 0.5f + vehicleDesc.wheelRadius + 5.0f), 60), initQuat);
	startTransformPlayer3 = PxTransform(PxVec3(-31, (vehicleDesc.chassisDims.y * 0.5f + vehicleDesc.wheelRadius + 5.0f), 60), initQuat);
	startTransformPlayer4 = PxTransform(PxVec3(-11.5, (vehicleDesc.chassisDims.y * 0.5f + vehicleDesc.wheelRadius + 5.0f), 60), initQuat);
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

	vehicles.clear(); // idk if this is needed
	vehicles.push_back(mVehiclePlayer1);
	vehicles.push_back(mVehiclePlayer2);
	vehicles.push_back(mVehiclePlayer3);
	vehicles.push_back(mVehiclePlayer4);

	randomizeIngredientLocations();
}

void PhysicsSystem::randomizeIngredientLocations()
{
	// Randomize starting location
	std::random_device r;
	std::seed_seq seed{ r(), r(), r(), r(), r(), r(), r(), r() };
	std::mt19937 eng(seed);

	std::shuffle(std::begin(spawnLocations), std::end(spawnLocations), eng);

	glm::vec3 loc;

	// FOOD ITEMS ---------------------------------------------------------------------------------------------------------------------
	// Note: Physx actor name must match Entity name

	// CHEESE
	loc = LOCATIONS[spawnLocations[0]];
	PxTransform cheeseTransform(PxVec3(loc.x, loc.y, loc.z));
	this->cheese = createFoodBlock(cheeseTransform, 5.894f, 3.580f, 5.491f, "cheese");
	g_scene.getEntity("cheese")->originalSpawn = cheeseTransform;

	// SAUSAGE
	loc = LOCATIONS[spawnLocations[1]];
	PxTransform sausageTransform(PxVec3(loc.x, loc.y, loc.z));
	this->sausage = createFoodBlock(sausageTransform, 6.72f, 1.53f, 1.25f, "sausage");
	g_scene.getEntity("sausage")->originalSpawn = sausageTransform;

	// TOMATO
	loc = LOCATIONS[spawnLocations[2]];
	PxTransform tomatoTransform(PxVec3(loc.x, loc.y, loc.z));
	this->tomato = createFoodBlock(tomatoTransform, 6.11f, 5.49f, 6.11f, "tomato");
	g_scene.getEntity("tomato")->originalSpawn = tomatoTransform;

	// DOUGH
	loc = LOCATIONS[spawnLocations[3]];
	PxTransform doughTransform(PxVec3(loc.x, loc.y, loc.z));
	this->dough = createFoodBlock(doughTransform, 5.69f, 2.66f, 5.69f, "dough");
	g_scene.getEntity("dough")->originalSpawn = doughTransform;

	// CARROT
	loc = LOCATIONS[spawnLocations[4]];
	PxTransform carrotTransform(PxVec3(loc.x, loc.y, loc.z));
	this->carrot = createFoodBlock(carrotTransform, 9.32f, 2.76f, 2.32f, "carrot");
	g_scene.getEntity("carrot")->originalSpawn = carrotTransform;

	// LETTUCE
	loc = LOCATIONS[spawnLocations[5]];
	PxTransform lettuceTransform(PxVec3(loc.x, loc.y, loc.z));
	this->lettuce = createFoodBlock(lettuceTransform, 6.45f, 4.65f, 6.18f, "lettuce");
	g_scene.getEntity("lettuce")->originalSpawn = lettuceTransform;

	// PARSNIP
	loc = LOCATIONS[spawnLocations[6]];
	PxTransform parsnipTransform(PxVec3(loc.x, loc.y, loc.z));
	this->parsnip = createFoodBlock(parsnipTransform, 6.06f, 7.08f, 6.06f, "parsnip");
	g_scene.getEntity("parsnip")->originalSpawn = parsnipTransform;

	// RICE
	loc = LOCATIONS[spawnLocations[7]];
	PxTransform riceTransform(PxVec3(loc.x, loc.y, loc.z));
	this->rice = createFoodBlock(riceTransform, 6.65f, 4.48f, 6.61f, "rice");
	g_scene.getEntity("rice")->originalSpawn = riceTransform;

	// EGG
	loc = LOCATIONS[spawnLocations[8]];
	PxTransform eggTransform(PxVec3(loc.x, loc.y, loc.z));
	this->egg = createFoodBlock(eggTransform, 5.14f, 0.683f, 5.14f, "egg");
	g_scene.getEntity("egg")->originalSpawn = eggTransform;

	// CHICKEN
	loc = LOCATIONS[spawnLocations[9]];
	PxTransform chickenTransform(PxVec3(loc.x, loc.y, loc.z));
	this->chicken = createFoodBlock(chickenTransform, 7.83f, 2.58f, 4.49f, "chicken");
	g_scene.getEntity("chicken")->originalSpawn = chickenTransform;

	// PEAS
	loc = LOCATIONS[spawnLocations[10]];
	PxTransform peasTransform(PxVec3(loc.x, loc.y, loc.z));
	this->peas = createFoodBlock(peasTransform, 7.13f, 2.18f, 2.41f, "peas");
	g_scene.getEntity("peas")->originalSpawn = peasTransform;
}

// Constructor
PhysicsSystem::PhysicsSystem() :
	chassis_mass(400.f)
	, peak_torque(100000.f)
	, max_omega(1000.f)
	, wheel_mass(800.f)
	, wheel_damping_rate(20.f)
	, wheel_moi(100.f)
	, chassis_moi_y(0.25f)
	, max_brake_torque(100000.f)
	, max_hand_brake_torque(15000.f)
	, camber_at_rest(0.2f)
	, spring_strength(35000.f)
	, max_compression(.5f)
	, max_droop(0.1f)
	, max_steer(PI/2.f)
{
	// Foundation
	this->mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, mDefaultAllocatorCallback, mDefaultErrorCallback);
	if (!mFoundation) { std::cout << "PxCreateFoundation failed!" << std::endl; assert(mFoundation); }

	// PVD
	this->mPvd = PxCreatePvd(*mFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	mPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	// Physics
	PxTolerancesScale tolerances = PxTolerancesScale();
	tolerances.length = 10;
	tolerances.speed = 10;
	this->mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, tolerances, true, mPvd);
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

void PhysicsSystem::updateVehicle(PxVehicleDrive4W* player, bool& isVehicleInAir, PxVehicleDrive4WRawInputData& inputData, std::string entityName, const float timestep, int gameStage) {
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
	if (gameStage != 5) {
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

void PhysicsSystem::update(const float dt, int gameStage)
{
	// Only update if more than 1/120th of a second has passed since last update
	// Making the constant much larger than 1/120th causes a significant jitter
	float timestep = 1.0f / 120.0f;
	this->mAccumulator += dt;
	if (this->mAccumulator < timestep)
		return;

	this->mAccumulator -= timestep;

	PxTransform trans = PxTransform(PxVec3(0, -100, 0));
	if (g_scene.getEntity("tomato")->removeFlag)
		this->tomato->setGlobalPose(trans);
	if (g_scene.getEntity("cheese")->removeFlag)
		this->cheese->setGlobalPose(trans);
	if (g_scene.getEntity("sausage")->removeFlag)
		this->sausage->setGlobalPose(trans);
	if (g_scene.getEntity("dough")->removeFlag)
		this->dough->setGlobalPose(trans);
	if (g_scene.getEntity("carrot")->removeFlag)
		this->carrot->setGlobalPose(trans);
	if (g_scene.getEntity("lettuce")->removeFlag)
		this->lettuce->setGlobalPose(trans);
	if (g_scene.getEntity("parsnip")->removeFlag)
		this->parsnip->setGlobalPose(trans);
	if (g_scene.getEntity("rice")->removeFlag)
		this->rice->setGlobalPose(trans);
	if (g_scene.getEntity("egg")->removeFlag)
		this->egg->setGlobalPose(trans);
	if (g_scene.getEntity("chicken")->removeFlag)
		this->chicken->setGlobalPose(trans);
	if (g_scene.getEntity("peas")->removeFlag)
		this->peas->setGlobalPose(trans);

	resetOutOfBoundsObjects();

	trans = this->mVehiclePlayer1->getRigidDynamicActor()->getGlobalPose();
	g_scene.getEntity("player1")->speed = (float)mVehiclePlayer1->computeForwardSpeed();
	glm::quat newQuat = glm::quat(trans.q.w, trans.q.x, trans.q.y, trans.q.z);
	glm::mat4 rotationMat = toMat4(newQuat);
	g_scene.getEntity("player1")->orientation = rotationMat * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f);
	g_scene.getEntity("player2")->speed = (float)mVehiclePlayer2->computeForwardSpeed();
	g_scene.getEntity("player3")->speed = (float)mVehiclePlayer3->computeForwardSpeed();
	g_scene.getEntity("player4")->speed = (float)mVehiclePlayer4->computeForwardSpeed();

	// Update scene in physics simulation
	this->mScene->simulate(timestep);
	this->mScene->fetchResults(true);

	// Update the players
	updateVehicle(this->mVehiclePlayer1, this->mIsVehicleInAirPlayer1, this->mVehicleInputDataPlayer1, "player1", timestep, gameStage);
	updateVehicle(this->mVehiclePlayer2, this->mIsVehicleInAirPlayer2, this->mVehicleInputDataPlayer2, "player2", timestep, gameStage);
	updateVehicle(this->mVehiclePlayer3, this->mIsVehicleInAirPlayer3, this->mVehicleInputDataPlayer3, "player3", timestep, gameStage);
	updateVehicle(this->mVehiclePlayer4, this->mIsVehicleInAirPlayer4, this->mVehicleInputDataPlayer4, "player4", timestep, gameStage);

	// Update kitchen position
	g_scene.getEntity("countertop")->getTransform()->update(kitchen->getGlobalPose());

	// Update the food transforms
	updateFoodTransforms();

	// Detect if cameras go behind walls
	if (g_scene.numPlayers > 3)
		raycastCamera(this->mVehiclePlayer4, "player4");
	if (g_scene.numPlayers > 2)
		raycastCamera(this->mVehiclePlayer3, "player3");
	if (g_scene.numPlayers > 1)
		raycastCamera(this->mVehiclePlayer2, "player2");
	raycastCamera(this->mVehiclePlayer1, "player1");

	checkIngredientCollisoins();
}

// Check if camera is behind a wall and save the position of the wall if it is
void PhysicsSystem::raycastCamera(physx::PxVehicleDrive4W* vehicle, std::string name)
{
	Camera* camera = nullptr;

	if (name == "player1")
		camera = g_scene.p1Camera;
	else if (name == "player2")
		camera = g_scene.p2Camera;
	else if (name == "player3")
		camera = g_scene.p3Camera;
	else if (name == "player4")
		camera = g_scene.p4Camera;
	else
		return; // Uh-oh, something has gone wrong

	// Get this player's current position
	physx::PxTransform trans = vehicle->getRigidDynamicActor()->getGlobalPose();
	trans.p.y += 2.0f;

	// Shoot a ray from just above the player to their camera to detect walls
	glm::vec3 playerPos = glm::vec3(trans.p.x, trans.p.y, trans.p.z);
	glm::vec3 unitDir = glm::normalize(camera->position - playerPos);
	float distance = glm::distance(camera->position, playerPos);
	physx::PxVec3 unitDirPx = physx::PxVec3(unitDir.x, unitDir.y, unitDir.z);
	physx::PxRaycastBuffer hit;
	bool status = this->mScene->raycast(trans.p, unitDirPx, distance, hit);

	// If there was a wall between the player and their camera, save the wall's position
	// to use as the camera's new position for rendering
	if (status)
	{
		if (name == "player1")
		{
			this->p1CameraHitPos = glm::vec3(hit.block.position.x, hit.block.position.y, hit.block.position.z);
			this->p1CameraHit = true;
		}
		else if (name == "player2")
		{
			this->p2CameraHitPos = glm::vec3(hit.block.position.x, hit.block.position.y, hit.block.position.z);
			this->p2CameraHit = true;
		}
		else if (name == "player3")
		{
			this->p3CameraHitPos = glm::vec3(hit.block.position.x, hit.block.position.y, hit.block.position.z);
			this->p3CameraHit = true;
		}
		else if (name == "player4")
		{
			this->p3CameraHitPos = glm::vec3(hit.block.position.x, hit.block.position.y, hit.block.position.z);
			this->p3CameraHit = true;
		}
		else
			return; // Uh-oh, something has gone wrong
	}
	// If there was no wall, do nothing
	else
	{
		this->p1CameraHit = false;
		this->p2CameraHit = false;
		this->p3CameraHit = false;
		this->p4CameraHit = false;
	}
}

void PhysicsSystem::updateFoodTransforms(bool setAllVisible)
{

	glm::quat q = glm::quat(glm::vec3(0.0f, glfwGetTime(), 0.0f));
	PxTransform trans = PxTransform(PxQuat(q.w, q.x, q.y, q.z));

	// Cheese
	this->cheese->setGlobalPose(this->cheese->getGlobalPose().transform(trans));
	g_scene.getEntity("cheese")->getTransform()->update(this->cheese->getGlobalPose());

	// Sausage
	this->sausage->setGlobalPose(this->sausage->getGlobalPose().transform(trans));
	g_scene.getEntity("sausage")->getTransform()->update(this->sausage->getGlobalPose());

	// Tomato
	this->tomato->setGlobalPose(this->tomato->getGlobalPose().transform(trans));
	g_scene.getEntity("tomato")->getTransform()->update(this->tomato->getGlobalPose());

	// Dough
	this->dough->setGlobalPose(this->dough->getGlobalPose().transform(trans));
	g_scene.getEntity("dough")->getTransform()->update(this->dough->getGlobalPose());

	// Carrot
	this->carrot->setGlobalPose(this->carrot->getGlobalPose().transform(trans));
	g_scene.getEntity("carrot")->getTransform()->update(this->carrot->getGlobalPose());

	// Lettuce
	this->lettuce->setGlobalPose(this->lettuce->getGlobalPose().transform(trans));
	g_scene.getEntity("lettuce")->getTransform()->update(this->lettuce->getGlobalPose());

	// Parsnip
	this->parsnip->setGlobalPose(this->parsnip->getGlobalPose().transform(trans));
	g_scene.getEntity("parsnip")->getTransform()->update(this->parsnip->getGlobalPose());

	// Rice
	this->rice->setGlobalPose(this->rice->getGlobalPose().transform(trans));
	g_scene.getEntity("rice")->getTransform()->update(this->rice->getGlobalPose());

	// Egg
	this->egg->setGlobalPose(this->egg->getGlobalPose().transform(trans));
	g_scene.getEntity("egg")->getTransform()->update(this->egg->getGlobalPose());

	// Chicken
	this->chicken->setGlobalPose(this->chicken->getGlobalPose().transform(trans));
	g_scene.getEntity("chicken")->getTransform()->update(this->chicken->getGlobalPose());

	// Peas
	this->peas->setGlobalPose(this->peas->getGlobalPose().transform(trans));
	g_scene.getEntity("peas")->getTransform()->update(this->peas->getGlobalPose());

	// Check if visibility needs to be reset
	if (setAllVisible)
	{
		g_scene.getEntity("cheese")->removeFlag = false;
		g_scene.getEntity("sausage")->removeFlag = false;
		g_scene.getEntity("tomato")->removeFlag = false;
		g_scene.getEntity("dough")->removeFlag = false;
		g_scene.getEntity("carrot")->removeFlag = false;
		g_scene.getEntity("lettuce")->removeFlag = false;
		g_scene.getEntity("parsnip")->removeFlag = false;
		g_scene.getEntity("rice")->removeFlag = false;
		g_scene.getEntity("egg")->removeFlag = false;
		g_scene.getEntity("chicken")->removeFlag = false;
		g_scene.getEntity("peas")->removeFlag = false;
	}
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

float PhysicsSystem::getPlayerSidewaysSpeed(int playerNumber)
{
	float result;

	switch (playerNumber)
	{
	case (1):
		result = (float)mVehiclePlayer1->computeSidewaysSpeed();
		return result;
		break;
	case (2):
		result = (float)mVehiclePlayer2->computeSidewaysSpeed();
		return result;
		break;
	case (3):
		result = (float)mVehiclePlayer3->computeSidewaysSpeed();
		return result;
		break;
	case (4):
		result = (float)mVehiclePlayer4->computeSidewaysSpeed();
		return result;
		break;
	default:
		break;
	}
}

bool PhysicsSystem::getIsVehicleInAir(int playerNumber) {
	switch (playerNumber)
	{
	case (1):
		return mIsVehicleInAirPlayer1;
		break;
	case (2):
		return mIsVehicleInAirPlayer2;
		break;
	case (3):
		return mIsVehicleInAirPlayer3;
		break;
	case (4):
		return mIsVehicleInAirPlayer4;
		break;
	default:
		break;
	}
	return false;
}

void PhysicsSystem::resetOutOfBoundsObjects() 
{
	// Players
	PxTransform reset = PxTransform(PxVec3(0, 5.0, 0));
	if (this->mVehiclePlayer1->getRigidDynamicActor()->getGlobalPose().p.y < -75.0f)
	{
		this->mVehiclePlayer1->getRigidDynamicActor()->setGlobalPose(reset);
		g_scene.getEntity("player1")->getInventory()->clearRandomIngredient();
	}
	if (this->mVehiclePlayer2->getRigidDynamicActor()->getGlobalPose().p.y < -75.0f)
	{
		this->mVehiclePlayer2->getRigidDynamicActor()->setGlobalPose(reset);
		g_scene.getEntity("player2")->getInventory()->clearRandomIngredient();
	}
	if (this->mVehiclePlayer3->getRigidDynamicActor()->getGlobalPose().p.y < -75.0f)
	{
		this->mVehiclePlayer3->getRigidDynamicActor()->setGlobalPose(reset);
		g_scene.getEntity("player3")->getInventory()->clearRandomIngredient();
	}
	if (this->mVehiclePlayer4->getRigidDynamicActor()->getGlobalPose().p.y < -75.0f)
	{
		this->mVehiclePlayer4->getRigidDynamicActor()->setGlobalPose(reset);
		g_scene.getEntity("player4")->getInventory()->clearRandomIngredient();
	}

	// Ingredients
	if (this->cheese->getGlobalPose().p.y < -75.0f && this->cheese->getGlobalPose().p.y > -99.0f)
		this->cheese->setGlobalPose(g_scene.getEntity("cheese")->originalSpawn);
	if (this->sausage->getGlobalPose().p.y < -75.0f && this->sausage->getGlobalPose().p.y > -99.0f)
		this->sausage->setGlobalPose(g_scene.getEntity("sausage")->originalSpawn);
	if (this->tomato->getGlobalPose().p.y < -75.0f && this->tomato->getGlobalPose().p.y > -99.0f)
		this->tomato->setGlobalPose(g_scene.getEntity("tomato")->originalSpawn);
	if (this->dough->getGlobalPose().p.y < -75.0f && this->dough->getGlobalPose().p.y > -99.0f)
		this->dough->setGlobalPose(g_scene.getEntity("dough")->originalSpawn);
	if (this->carrot->getGlobalPose().p.y < -75.0f && this->carrot->getGlobalPose().p.y > -99.0f)
		this->carrot->setGlobalPose(g_scene.getEntity("carrot")->originalSpawn);
	if (this->lettuce->getGlobalPose().p.y < -75.0f && this->lettuce->getGlobalPose().p.y > -99.0f)
		this->lettuce->setGlobalPose(g_scene.getEntity("lettuce")->originalSpawn);
	if (this->parsnip->getGlobalPose().p.y < -75.0f && this->parsnip->getGlobalPose().p.y > -99.0f)
		this->parsnip->setGlobalPose(g_scene.getEntity("parsnip")->originalSpawn);
	if (this->rice->getGlobalPose().p.y < -75.0f && this->rice->getGlobalPose().p.y > -99.0f)
		this->rice->setGlobalPose(g_scene.getEntity("rice")->originalSpawn);
	if (this->egg->getGlobalPose().p.y < -75.0f && this->egg->getGlobalPose().p.y > -99.0f)
		this->egg->setGlobalPose(g_scene.getEntity("egg")->originalSpawn);
	if (this->chicken->getGlobalPose().p.y < -75.0f && this->chicken->getGlobalPose().p.y > -99.0f)
		this->chicken->setGlobalPose(g_scene.getEntity("chicken")->originalSpawn);
	if (this->peas->getGlobalPose().p.y < -75.0f && this->peas->getGlobalPose().p.y > -99.0f)
		this->peas->setGlobalPose(g_scene.getEntity("peas")->originalSpawn);

	updateFoodTransforms();
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
			//printf("Cooldown for %s is not over yet.\n", firstActor->name.c_str());
		}
	}
	else if (buf.nbTouches < buf2.nbTouches || firstVehicle->getRigidDynamicActor()->getLinearVelocity() == PxVec3(0.0f)) {
		if (currentTime - secondActor->collisionCooldownStart > cooldownThreshold) {
			secondActor->collisionCooldownStart = -1.0;
			collisionWinner = 2;
		}
		else {
			cooldownIncomplete = true;
			//printf("Cooldown for %s is not over yet.\n", secondActor->name.c_str());
		}
	}
	

	// RESOLVE COLLISION INGREDIENT EXCHANGE ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	if (collisionWinner == 0) {
		// No winners (headon collision)
		if (!cooldownIncomplete);
			//printf("No collision winners. No ingredients moved.\n");
	}
	else {
		Inventory* player1Inventory = (Inventory*)firstActor->getComponent("inventory");
		Inventory* player2Inventory = (Inventory*)secondActor->getComponent("inventory");
		if (collisionWinner == 1) {
			//int ingredient = player2Inventory->removeRandomPizzaIngredient(player1Inventory->cheese, player1Inventory->dough, player1Inventory->sausage, player1Inventory->tomato);
			//player1Inventory->setIngredientFromId(ingredient);
			firstActor->collisionCooldownStart = glfwGetTime();
			//printf("%s took food id %d from %s\n", firstActor->name.c_str(), ingredient, secondActor->name.c_str());
			
		}
		else if (collisionWinner == 2) {
			//int ingredient = player1Inventory->removeRandomPizzaIngredient(player2Inventory->cheese, player2Inventory->dough, player2Inventory->sausage, player2Inventory->tomato);
			//player2Inventory->setIngredientFromId(ingredient);
			secondActor->collisionCooldownStart = glfwGetTime();
			//printf("%s took food id %d from %s\n\n", secondActor->name.c_str(), ingredient, firstActor->name.c_str());
			
		}
	}

	// RESET FLAGS
	firstActor->verifyPlayerCollision = false;
	secondActor->verifyPlayerCollision = false;
	firstActor->otherPlayerInCollision = "";
	secondActor->otherPlayerInCollision = "";
}

// TODO: DELETE THIS FUNCTION
void PhysicsSystem::magnet(int stealer_id)
{
	// Get the entity that is stealing, and the entities that can be stolen from
	Entity* stealer;
	PxVehicleDrive4W* stealer_vehicle;

	std::vector<PxVehicleDrive4W*> victim_vehicles = {
		mVehiclePlayer1,
		mVehiclePlayer2,
		mVehiclePlayer3,
		mVehiclePlayer4 };

	std::vector<Entity*> victims = {
		g_scene.getEntity("player1"),
		g_scene.getEntity("player2"),
		g_scene.getEntity("player3"),
		g_scene.getEntity("player4") };
	switch (stealer_id) {
	case 1:
		stealer = g_scene.getEntity("player1");
		stealer_vehicle = mVehiclePlayer1;
		victims.erase(victims.begin());
		victim_vehicles.erase(victim_vehicles.begin());
		break;
	case 2:
		stealer = g_scene.getEntity("player2");
		stealer_vehicle = mVehiclePlayer2;
		victims.erase(victims.begin() + 1);
		victim_vehicles.erase(victim_vehicles.begin() + 1);
		break;
	case 3:
		stealer = g_scene.getEntity("player3");
		stealer_vehicle = mVehiclePlayer3;
		victims.erase(victims.begin() + 2);
		victim_vehicles.erase(victim_vehicles.begin() + 2);
		break;
	case 4:
		stealer = g_scene.getEntity("player4");
		stealer_vehicle = mVehiclePlayer4;
		victims.erase(victims.begin() + 3);
		victim_vehicles.erase(victim_vehicles.begin() + 3);
		break;
	default:
		return;
		break;
	}

	// See if the stealer can steal (cooldown)
	float currentTime = glfwGetTime();
	if (currentTime - stealer->lastMagnetUse < stealer->magnetCooldown) return;



	PxVec3 stealer_pos = stealer_vehicle->getRigidDynamicActor()->getGlobalPose().p;
	auto stealer_recipe = (Recipe*)stealer->getComponent("recipe");
	auto stealer_inventory = (Inventory*)stealer->getComponent("inventory");
	bool playBanner = false;
	for (int i = 0; i < victims.size();) {
		//TODO: right now only the location is checked
		// See the victims that are in range
		PxVec3 victim_pos = victim_vehicles[i]->getRigidDynamicActor()->getGlobalPose().p;
		if ((victim_pos - stealer_pos).magnitudeSquared() > stealer->magnetDistanceSquared) {
			victims.erase(victims.begin() + i);
			victim_vehicles.erase(victim_vehicles.begin() + i);
			continue;
		}

		// See if victim can be stolen from
		if (currentTime - victims[i]->lastGracePeriodStart < stealer->gracePeriod) {
			victims.erase(victims.begin() + i);
			victim_vehicles.erase(victim_vehicles.begin() + i);
			continue;
		}

		// Steal item in our recipe if victim has one
		// This is extremely messy but oh well
		auto victim_inventory = (Inventory*)victims[i]->getComponent("inventory");
		boolean stole = false;
		for (Ingredient ing : stealer_recipe->list) {
			switch (ing) {
			case Cheese:
				if (stealer_inventory->cheese != 0) break; // already have this no need to steal
				if (victim_inventory->cheese != 0) {
					victim_inventory->cheese--;
					stealer_inventory->cheese++;
					stole = true;
				}
				break;
			case Dough:
				if (stealer_inventory->dough != 0) break; // already have this no need to steal
				if (victim_inventory->dough != 0) {
					victim_inventory->dough--;
					stealer_inventory->dough++;
					stole = true;
				}
				break;
			case Sausage:
				if (stealer_inventory->sausage != 0) break; // already have this no need to steal
				if (victim_inventory->sausage != 0) {
					victim_inventory->sausage--;
					stealer_inventory->sausage++;
					stole = true;
				}
				break;
			case Tomato:
				if (stealer_inventory->tomato != 0) break; // already have this no need to steal
				if (victim_inventory->tomato != 0) {
					victim_inventory->tomato--;
					stealer_inventory->tomato++;
					stole = true;
				}
				break;
			case Carrot:
				if (stealer_inventory->carrot != 0) break; // already have this no need to steal
				if (victim_inventory->carrot != 0) {
					victim_inventory->carrot--;
					stealer_inventory->carrot++;
					stole = true;
				}
				break;
			case Lettuce:
				if (stealer_inventory->lettuce != 0) break; // already have this no need to steal
				if (victim_inventory->lettuce != 0) {
					victim_inventory->lettuce--;
					stealer_inventory->lettuce++;
					stole = true;
				}
				break;
			case Parsnip:
				if (stealer_inventory->parsnip != 0) break; // already have this no need to steal
				if (victim_inventory->parsnip != 0) {
					victim_inventory->parsnip--;
					stealer_inventory->parsnip++;
					stole = true;
				}
				break;
			case Rice:
				if (stealer_inventory->rice != 0) break; // already have this no need to steal
				if (victim_inventory->rice != 0) {
					victim_inventory->rice--;
					stealer_inventory->rice++;
					stole = true;
				}
				break;
			case Egg:
				if (stealer_inventory->egg != 0) break; // already have this no need to steal
				if (victim_inventory->egg != 0) {
					victim_inventory->egg--;
					stealer_inventory->egg++;
					stole = true;
				}
				break;
			case Chicken:
				if (stealer_inventory->chicken != 0) break; // already have this no need to steal
				if (victim_inventory->chicken != 0) {
					victim_inventory->chicken--;
					stealer_inventory->chicken++;
					stole = true;
				}
				break;
			case Peas:
				if (stealer_inventory->peas != 0) break; // already have this no need to steal
				if (victim_inventory->peas != 0) {
					victim_inventory->peas--;
					stealer_inventory->peas++;
					stole = true;
				}
				break;
			}
			if (stole) {
				stealer->lastMagnetUse = currentTime;
				stealer->lastGracePeriodStart = currentTime; // Prevents instant stealbacks
				victims[i]->lastGracePeriodStart = currentTime;
				if (stealer_id == 1)
				{
					/*if (!stealer->bannerSoundPlayed && stealer_inventory->allIngredientsCollected) {
						playBanner = true;
						stealer->bannerSoundPlayed = true;
					}*/
					g_systems.audio->endSlurp(stealer->getAudioSource(), playBanner, true);
				}
				if (victims[i] == g_scene.getEntity("player1")) // TODO: This needs to change for multiplayer
				{
					g_systems.audio->ingredientSuck(stealer->getAudioSource());
				}
				return;
			}
		}

		++i;
	}

	stealer->lastMagnetUse = currentTime;
	if (stealer_id == 1)
	{
		/*if (!stealer->bannerSoundPlayed && stealer_inventory->allIngredientsCollected) {
			playBanner = true;
			stealer->bannerSoundPlayed = true;
		}*/
		g_systems.audio->endSlurp(stealer->getAudioSource(), playBanner, false);
	}
}

void PhysicsSystem::magnetCheckStealing(int stealer_id, bool steal_button_held, bool steal_button_just_pressed) {
	// setting up the stealer entity (see if this can be passed in instead)
	Entity* stealer;
	bool playBanner = false;
	switch (stealer_id) {
	case 1:
		stealer = g_scene.getEntity("player1");
		break;
	case 2:
		stealer = g_scene.getEntity("player2");
		break;
	case 3:
		stealer = g_scene.getEntity("player3");
		break;
	case 4:
		stealer = g_scene.getEntity("player4");
		break;
	default:
		return; // Id passed in does not correspond to a car
	}

	// Check if in Cooldown
	float currentTime = glfwGetTime();
	if (currentTime - stealer->lastMagnetUse < stealer->magnetCooldown) { // in Cooldown
		if (steal_button_held && steal_button_just_pressed) {
			// TODO: play a cant steal right now noise (might be annoying actually)
		}
		stealer->magnetStatus = 0;
	}
	else { // not in cooldown
		// Trying to start stealing
		if (steal_button_held && steal_button_just_pressed) {
			makeVictimsList(stealer_id, stealer->tethered_victims); // TODO: THIS MIGHT NOT HAVE TO BE A FUNCTION
			checkVictims(stealer, stealer->tethered_victims, currentTime);
			if (!stealer->tethered_victims.empty()) { // Check if stealer has victims
				g_systems.audio->playSlurp(stealer->getAudioSource());
				stealer->magnetStartTime = currentTime;
				stealer->tethered = true;
				stealer->magnetStatus = 3;
			}
			else {
				/*if (!stealer->bannerSoundPlayed && stealer->getInventory()->allIngredientsCollected) {
					playBanner = true;
					stealer->bannerSoundPlayed = true;
				}*/
				g_systems.audio->endSlurp(stealer->getAudioSource(), playBanner, false);
				stealer->lastMagnetUse = currentTime;
				stealer->magnetStatus = 0;
			}
			return;
		}

		// Check if currently tethered
		if (stealer->tethered) {
			if (steal_button_held) {
				// Check if tethers still hold
				checkVictims(stealer, stealer->tethered_victims, currentTime);
				if (!stealer->tethered_victims.empty()) {
					if (currentTime - stealer->magnetStartTime > stealer->magnetTimeToSteal) { // Stole succesfully
						// Pick a victim (should usually be one but to handle the edge case with more players)
						auto stealer_recipe = (Recipe*)stealer->getComponent("recipe");

						Entity* victim = stealer->tethered_victims[rand() % stealer->tethered_victims.size()];
						auto victim_inventory = (Inventory*)victim->getComponent("inventory");
						auto stealer_inventory = (Inventory*)stealer->getComponent("inventory");
						bool stolen = false;
						// STEAL
						for (Ingredient ing : stealer_recipe->list) {
							switch (ing) {
							case Cheese:
								if (stealer_inventory->cheese != 0) break;
								if (victim_inventory->cheese != 0) {
									victim_inventory->cheese--;
									stealer_inventory->cheese++;
									stolen = true;
								}
								break;
							case Dough:
								if (stealer_inventory->dough != 0) break;
								if (victim_inventory->dough != 0) {
									victim_inventory->dough--;
									stealer_inventory->dough++;
									stolen = true;
								}
								break;
							case Sausage:
								if (stealer_inventory->sausage != 0) break;
								if (victim_inventory->sausage != 0) {
									victim_inventory->sausage--;
									stealer_inventory->sausage++;
									stolen = true;
								}
								break;
							case Tomato:
								if (stealer_inventory->tomato != 0) break;
								if (victim_inventory->tomato != 0) {
									victim_inventory->tomato--;
									stealer_inventory->tomato++;
									stolen = true;
								}
								break;
							case Carrot:
								if (stealer_inventory->carrot != 0) break;
								if (victim_inventory->carrot != 0) {
									victim_inventory->carrot--;
									stealer_inventory->carrot++;
									stolen = true;
								}
								break;
							case Lettuce:
								if (stealer_inventory->lettuce != 0) break;
								if (victim_inventory->lettuce != 0) {
									victim_inventory->lettuce--;
									stealer_inventory->lettuce++;
									stolen = true;
								}
								break;
							case Parsnip:
								if (stealer_inventory->parsnip != 0) break;
								if (victim_inventory->parsnip != 0) {
									victim_inventory->parsnip--;
									stealer_inventory->parsnip++;
									stolen = true;
								}
								break;
							case Rice:
								if (stealer_inventory->rice != 0) break;
								if (victim_inventory->rice != 0) {
									victim_inventory->rice--;
									stealer_inventory->rice++;
									stolen = true;
								}
								break;
							case Egg:
								if (stealer_inventory->egg != 0) break;
								if (victim_inventory->egg != 0) {
									victim_inventory->egg--;
									stealer_inventory->egg++;
									stolen = true;
								}
								break;
							case Chicken:
								if (stealer_inventory->chicken != 0) break;
								if (victim_inventory->chicken != 0) {
									victim_inventory->chicken--;
									stealer_inventory->chicken++;
									stolen = true;
								}
								break;
							case Peas:
								if (stealer_inventory->peas != 0) break;
								if (victim_inventory->peas != 0) {
									victim_inventory->peas--;
									stealer_inventory->peas++;
									stolen = true;
								}
								break;
							}
							if (stolen) {
								break;
							}
						}
						/*if (!stealer->bannerSoundPlayed && stealer->getInventory()->allIngredientsCollected) {
							playBanner = true;
							stealer->bannerSoundPlayed = true;
						}*/
						g_systems.audio->endSlurp(stealer->getAudioSource(), playBanner, true);

						// Update stealer/victim variables
						stealer->lastGracePeriodStart = currentTime;
						stealer->lastMagnetUse = currentTime;
						stealer->magnetStatus = 0;
						stealer->tethered = false;

						victim->lastGracePeriodStart = currentTime;
						return;
					}
					else {
						stealer->magnetStatus = 3; // still tethered
						return;
					}
				}
				else { // Tethered list is empty
					/*if (!stealer->bannerSoundPlayed && stealer->getInventory()->allIngredientsCollected) {
						playBanner = true;
						stealer->bannerSoundPlayed = true;
					}*/
					g_systems.audio->endSlurp(stealer->getAudioSource(), playBanner, false);
					stealer->tethered = false;
					stealer->lastMagnetUse = currentTime;
					stealer->magnetStatus = 0;
					return;
				}
			}
			else {
				/*if (!stealer->bannerSoundPlayed && stealer->getInventory()->allIngredientsCollected) {
					playBanner = true;
					stealer->bannerSoundPlayed = true;
				}*/
				g_systems.audio->endSlurp(stealer->getAudioSource(), playBanner, false);
				stealer->tethered = false;
				stealer->tethered_victims.clear();
				stealer->lastMagnetUse = currentTime;
				stealer->magnetStatus = 0;
				return;
			}
		}

		// Check if there are stealer has victims
		std::vector<Entity*> victims;
		makeVictimsList(stealer_id, victims);
		checkVictims(stealer, victims, currentTime);
		if (!victims.empty()) { // there are possible victims
			stealer->magnetStatus = 2;
		}
		else {
			stealer->magnetStatus = 1;
		}
	}
}

void PhysicsSystem::checkVictims(Entity* stealer, std::vector<Entity*>& victims, float currentTime) {
	// Setup variables to reference easier
	glm::vec3 stealer_pos = stealer->getTransform()->position;//stealer->vehicle->getRigidDynamicActor()->getGlobalPose().p;
	auto stealer_recipe = (Recipe*)stealer->getComponent("recipe");
	auto stealer_inventory = (Inventory*)stealer->getComponent("inventory");
	for (int i = 0; i < victims.size();) {
		// See the victims that are in range
		glm::vec3 victim_pos = victims[i]->getTransform()->position;
		if (glm::length2(victim_pos - stealer_pos) > stealer->magnetDistanceSquared) {
			victims.erase(victims.begin() + i);
			continue;
		}

		// See if victim can be stolen from
		if (currentTime - victims[i]->lastGracePeriodStart < stealer->gracePeriod) {
			victims.erase(victims.begin() + i);
			continue;
		}

		// Steal item in our recipe if victim has one
		// This is extremely messy but oh well
		auto victim_inventory = (Inventory*)victims[i]->getComponent("inventory");
		bool has_item_of_interest = false;
		for (Ingredient ing : stealer_recipe->list) {
			switch (ing) {
			case Cheese:
				if (stealer_inventory->cheese != 0) break; 
				if (victim_inventory->cheese != 0) {
					has_item_of_interest = true;
				}
				break;
			case Dough:
				if (stealer_inventory->dough != 0) break; 
				if (victim_inventory->dough != 0) {
					has_item_of_interest = true;
				}
				break;
			case Sausage:
				if (stealer_inventory->sausage != 0) break; 
				if (victim_inventory->sausage != 0) {
					has_item_of_interest = true;
				}
				break;
			case Tomato:
				if (stealer_inventory->tomato != 0) break; 
				if (victim_inventory->tomato != 0) {
					has_item_of_interest = true;
				}
				break;
			case Carrot:
				if (stealer_inventory->carrot != 0) break; 
				if (victim_inventory->carrot != 0) {
					has_item_of_interest = true;
				}
				break;
			case Lettuce:
				if (stealer_inventory->lettuce != 0) break; 
				if (victim_inventory->lettuce != 0) {
					has_item_of_interest = true;
				}
				break;
			case Parsnip:
				if (stealer_inventory->parsnip != 0) break; 
				if (victim_inventory->parsnip != 0) {
					has_item_of_interest = true;
				}
				break;
			case Rice:
				if (stealer_inventory->rice != 0) break; 
				if (victim_inventory->rice != 0) {
					has_item_of_interest = true;
				}
				break;
			case Egg:
				if (stealer_inventory->egg != 0) break; 
				if (victim_inventory->egg != 0) {
					has_item_of_interest = true;
				}
				break;
			case Chicken:
				if (stealer_inventory->chicken != 0) break; 
				if (victim_inventory->chicken != 0) {
					has_item_of_interest = true;
				}
				break;
			case Peas:
				if (stealer_inventory->peas != 0) break; 
				if (victim_inventory->peas != 0) {
					has_item_of_interest = true;
				}
				break;
			}
		}
		if (!has_item_of_interest) {
			victims.erase(victims.begin() + i);
			continue;
		}
		++i;
	}
}

void PhysicsSystem::makeVictimsList(int stealer_id, std::vector<Entity*>& victims) {
	victims.clear();
	victims = {
	g_scene.getEntity("player1"),
	g_scene.getEntity("player2"),
	g_scene.getEntity("player3"),
	g_scene.getEntity("player4") };
	switch (stealer_id) {
	case 1:
		victims.erase(victims.begin());
		break;
	case 2:
		victims.erase(victims.begin() + 1);
		break;
	case 3:
		victims.erase(victims.begin() + 2);
		break;
	case 4:
		victims.erase(victims.begin() + 3);
		break;
	default:
		return; // Id passed in does not correspond to a car
	}
}

void PhysicsSystem::respawnPlayerInPlace(int playerNumber) {
	PxTransform placeTransform;
	switch (playerNumber)
	{
	case (1):
		placeTransform = mVehiclePlayer1->getRigidDynamicActor()->getGlobalPose();
		placeTransform.p.y += 3.0f;
		placeTransform.q.x = 0;
		placeTransform.q.z = 0;
		placeTransform.q.normalize();
		mVehiclePlayer1->getRigidDynamicActor()->setGlobalPose(placeTransform);
		mVehiclePlayer1->setToRestState();
		break;
	case (2):
		placeTransform = mVehiclePlayer2->getRigidDynamicActor()->getGlobalPose();
		placeTransform.p.y += 3.0f;
		placeTransform.q.x = 0;
		placeTransform.q.z = 0;
		placeTransform.q.normalize();
		mVehiclePlayer2->getRigidDynamicActor()->setGlobalPose(placeTransform);
		mVehiclePlayer2->setToRestState();
		break;
	case (3):
		placeTransform = mVehiclePlayer3->getRigidDynamicActor()->getGlobalPose();
		placeTransform.p.y += 3.0f;
		placeTransform.q.x = 0;
		placeTransform.q.z = 0;
		placeTransform.q.normalize();
		mVehiclePlayer3->getRigidDynamicActor()->setGlobalPose(placeTransform);
		mVehiclePlayer3->setToRestState();
		break;
	case (4):
		placeTransform = mVehiclePlayer4->getRigidDynamicActor()->getGlobalPose();
		placeTransform.p.y += 3.0f;
		placeTransform.q.x = 0;
		placeTransform.q.z = 0;
		placeTransform.q.normalize();
		mVehiclePlayer4->getRigidDynamicActor()->setGlobalPose(placeTransform);
		mVehiclePlayer4->setToRestState();
		break;
	default:
		break;
	}
}

std::vector<PxVehicleDrive4W*> PhysicsSystem::getVehicles()
{
	return vehicles;
}

void PhysicsSystem::checkIngredientCollisoins(){
	auto players = g_scene.getPlayers();
	for (int i = 0; i < players.size(); i++) {
		auto ingredients = players[i]->getCollectableIngredients();
		for (auto ingredient : ingredients) {
			//glm::vec3 distance2 = glm::length2(ingredient players[i]->getTransform()->position())
			if (players[i]->ingredientCollectDistanceSquared >= 
				glm::length2(ingredient->getTransform()->position-players[i]->getTransform()->position)) {
				players[i]->checkIngredientCollision(ingredient);
			}

		}
	}
}