#include <iostream>
#include <cassert>

#include "PhysicsSystem.h"

#include <snippetvehiclecommon/SnippetVehicleCreate.h>
#include <snippetvehiclecommon/SnippetVehicleFilterShader.h>

using namespace snippetvehicle;
using namespace physx;


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

enum DriveMode
{
	eDRIVE_MODE_ACCEL_FORWARDS = 0,
	eDRIVE_MODE_ACCEL_REVERSE,
	eDRIVE_MODE_HARD_TURN_LEFT,
	eDRIVE_MODE_HANDBRAKE_TURN_LEFT,
	eDRIVE_MODE_HARD_TURN_RIGHT,
	eDRIVE_MODE_HANDBRAKE_TURN_RIGHT,
	eDRIVE_MODE_BRAKE,
	eDRIVE_MODE_NONE
};

DriveMode gDriveModeOrder[] =
{
	eDRIVE_MODE_BRAKE,
	eDRIVE_MODE_ACCEL_FORWARDS,
	eDRIVE_MODE_BRAKE,
	eDRIVE_MODE_ACCEL_REVERSE,
	eDRIVE_MODE_BRAKE,
	eDRIVE_MODE_HARD_TURN_LEFT,
	eDRIVE_MODE_BRAKE,
	eDRIVE_MODE_HARD_TURN_RIGHT,
	eDRIVE_MODE_ACCEL_FORWARDS,
	eDRIVE_MODE_HANDBRAKE_TURN_LEFT,
	eDRIVE_MODE_ACCEL_FORWARDS,
	eDRIVE_MODE_HANDBRAKE_TURN_RIGHT,
	eDRIVE_MODE_NONE
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
	const PxF32 wheelMOI = 0.5f * wheelMass * wheelRadius * wheelRadius;
	const PxU32 nbWheels = 6;

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
	this->mDispatcher = mDispatcher = PxDefaultCpuDispatcherCreate(numWorkers);

	// Scene
	PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	sceneDesc.cpuDispatcher = mDispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
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

	// Add the ground plane to drive on
	PxFilterData groundPlaneSimFilterData(COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST, 0, 0);
	this->mGroundPlane = createDrivablePlane(groundPlaneSimFilterData, mMaterial, mPhysics);
	mScene->addActor(*mGroundPlane);

	//Create a vehicle that will drive on the plane.
	VehicleDesc vehicleDesc = initVehicleDesc(this->mMaterial);
	mVehicle4W = createVehicle4W(vehicleDesc, mPhysics, mCooking);
	PxTransform startTransform(PxVec3(0, (vehicleDesc.chassisDims.y * 0.5f + vehicleDesc.wheelRadius + 1.0f), 0), PxQuat(PxIdentity));
	mVehicle4W->getRigidDynamicActor()->setGlobalPose(startTransform);
	mScene->addActor(*mVehicle4W->getRigidDynamicActor());

	//Set the vehicle to rest in first gear.
	//Set the vehicle to use auto-gears.
	mVehicle4W->setToRestState();
	mVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
	mVehicle4W->mDriveDynData.setUseAutoGears(true);

	mVehicleModeTimer = 0.0f;
	mVehicleOrderProgress = 0;
	startBrakeMode();


	// Add some boxes
	unsigned int size = 10;
	float halfExtent = 5.0f;
	PxTransform t(PxVec3(0));
	PxShape* shape = mPhysics->createShape(PxBoxGeometry(halfExtent, halfExtent, halfExtent), *mMaterial);
	for (unsigned int i = 0; i < size; i++)
	{
		for (unsigned int j = 0; j < size; j++)
		{
			PxTransform localTm(PxVec3(PxReal(j * 2) - PxReal(size - i), PxReal(i * 2 + 1), 0) * halfExtent);
			PxRigidDynamic* body = mPhysics->createRigidDynamic(t.transform(localTm));
			body->attachShape(*shape);
			PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
			mScene->addActor(*body);
		}
	}
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


	// The following is necessary, but it all gets handled by the snippetvehicle code currently
	// We might want to handle this ourselves once we know what we're doing

	//const physx::PxU32 numWheels = 4;

	// Suspension strength and damping rate, wheel mass, tire stiffness, and suspension travel direction
	//physx::PxVehicleWheelsSimData* wheelsSimData = physx::PxVehicleWheelsSimData::allocate(numWheels);
	//setupWheelsSimulationData(wheelsSimData)

	// Engine peak torque, clutch strength, gearing ratios, and Ackermann steering correction
	//physx::PxVehicleDriveSimData4W driveSimData;
	//setupDriveSimData(driveSimData)

	// Needs geometry for wheels and chassis, mass, moment of inertia, and center of mass
	//physx::PxTransform t(physx::PxVec3(0, 0, 0));
	//physx::PxRigidDynamic* vehActor = mPhysics->createRigidDynamic(t);
	//setupVehicleActor(vehActor)
	//mScene->addActor(*vehActor);

	//physx::PxVehicleDrive4W* vehDrive4W = physx::PxVehicleDrive4W::allocate(numWheels);
	//vehDrive4W->setup(mPhysics, vehActor, *wheelsSimData, driveSimData, numWheels - 4);
}

void PhysicsSystem::update(float timeStep)
{
	mScene->simulate(timeStep);
	mScene->fetchResults(true);
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
	this->mVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eREVERSE);

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
		this->mVehicleInputData.setAnalogAccel(true);
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
		this->mVehicleInputData.setAnalogAccel(1.0f);
		this->mVehicleInputData.setAnalogSteer(1.0f);
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


/*
void incrementDrivingMode(const PxF32 timestep)
{
	gVehicleModeTimer += timestep;
	if (gVehicleModeTimer > gVehicleModeLifetime)
	{
		//If the mode just completed was eDRIVE_MODE_ACCEL_REVERSE then switch back to forward gears.
		if (eDRIVE_MODE_ACCEL_REVERSE == gDriveModeOrder[gVehicleOrderProgress])
		{
			gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
		}

		//Increment to next driving mode.
		gVehicleModeTimer = 0.0f;
		gVehicleOrderProgress++;
		releaseAllControls();

		//If we are at the end of the list of driving modes then start again.
		if (eDRIVE_MODE_NONE == gDriveModeOrder[gVehicleOrderProgress])
		{
			gVehicleOrderProgress = 0;
			gVehicleOrderComplete = true;
		}

		//Start driving in the selected mode.
		DriveMode eDriveMode = gDriveModeOrder[gVehicleOrderProgress];
		switch (eDriveMode)
		{
		case eDRIVE_MODE_ACCEL_FORWARDS:
			startAccelerateForwardsMode();
			break;
		case eDRIVE_MODE_ACCEL_REVERSE:
			startAccelerateReverseMode();
			break;
		case eDRIVE_MODE_HARD_TURN_LEFT:
			startTurnHardLeftMode();
			break;
		case eDRIVE_MODE_HANDBRAKE_TURN_LEFT:
			startHandbrakeTurnLeftMode();
			break;
		case eDRIVE_MODE_HARD_TURN_RIGHT:
			startTurnHardRightMode();
			break;
		case eDRIVE_MODE_HANDBRAKE_TURN_RIGHT:
			startHandbrakeTurnRightMode();
			break;
		case eDRIVE_MODE_BRAKE:
			startBrakeMode();
			break;
		case eDRIVE_MODE_NONE:
			break;
		};

		//If the mode about to start is eDRIVE_MODE_ACCEL_REVERSE then switch to reverse gears.
		if (eDRIVE_MODE_ACCEL_REVERSE == gDriveModeOrder[gVehicleOrderProgress])
		{
			gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eREVERSE);
		}
	}
}

void stepPhysics()
{
	const PxF32 timestep = 1.0f / 60.0f;

	//Cycle through the driving modes to demonstrate how to accelerate/reverse/brake/turn etc.
	incrementDrivingMode(timestep);

	//Update the control inputs for the vehicle.
	if (gMimicKeyInputs)
	{
		PxVehicleDrive4WSmoothDigitalRawInputsAndSetAnalogInputs(gKeySmoothingData, gSteerVsForwardSpeedTable, gVehicleInputData, timestep, gIsVehicleInAir, *gVehicle4W);
	}
	else
	{
		PxVehicleDrive4WSmoothAnalogRawInputsAndSetAnalogInputs(gPadSmoothingData, gSteerVsForwardSpeedTable, gVehicleInputData, timestep, gIsVehicleInAir, *gVehicle4W);
	}

	//Raycasts.
	PxVehicleWheels* vehicles[1] = { gVehicle4W };
	PxRaycastQueryResult* raycastResults = gVehicleSceneQueryData->getRaycastQueryResultBuffer(0);
	const PxU32 raycastResultsSize = gVehicleSceneQueryData->getQueryResultBufferSize();
	PxVehicleSuspensionRaycasts(gBatchQuery, 1, vehicles, raycastResultsSize, raycastResults);

	//Vehicle update.
	const PxVec3 grav = gScene->getGravity();
	PxWheelQueryResult wheelQueryResults[PX_MAX_NB_WHEELS];
	PxVehicleWheelQueryResult vehicleQueryResults[1] = { {wheelQueryResults, gVehicle4W->mWheelsSimData.getNbWheels()} };
	PxVehicleUpdates(timestep, grav, *gFrictionPairs, 1, vehicles, vehicleQueryResults);

	//Work out if the vehicle is in the air.
	gIsVehicleInAir = gVehicle4W->getRigidDynamicActor()->isSleeping() ? false : PxVehicleIsInAir(vehicleQueryResults[0]);

	//Scene update.
	gScene->simulate(timestep);
	gScene->fetchResults(true);
}

void cleanupPhysics()
{
	gVehicle4W->getRigidDynamicActor()->release();
	gVehicle4W->free();
	PX_RELEASE(gGroundPlane);
	PX_RELEASE(gBatchQuery);
	gVehicleSceneQueryData->free(gAllocator);
	PX_RELEASE(gFrictionPairs);
	PxCloseVehicleSDK();

	PX_RELEASE(gMaterial);
	PX_RELEASE(gCooking);
	PX_RELEASE(gScene);
	PX_RELEASE(gDispatcher);
	PX_RELEASE(gPhysics);
	if (gPvd)
	{
		PxPvdTransport* transport = gPvd->getTransport();
		gPvd->release();	gPvd = NULL;
		PX_RELEASE(transport);
	}
	PX_RELEASE(gFoundation);

	printf("SnippetVehicle4W done.\n");
}

void keyPress(unsigned char key, const PxTransform& camera)
{
	PX_UNUSED(camera);
	PX_UNUSED(key);
}*/