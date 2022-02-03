//#include <ctype.h>
//
//#include "PxPhysicsAPI.h"
//
//#include "vehicle/PxVehicleUtil.h"
//#include "../snippetvehiclecommon/SnippetVehicleSceneQuery.h"
//#include "../snippetvehiclecommon/SnippetVehicleFilterShader.h"
//#include "../snippetvehiclecommon/SnippetVehicleTireFriction.h"
//#include "../snippetvehiclecommon/SnippetVehicleCreate.h"
//
//#include "../snippetcommon/SnippetPrint.h"
//#include "../snippetcommon/SnippetPVD.h"
//#include "../snippetutils/SnippetUtils.h"
//
//
//
//VehicleDesc initVehicleDesc()
//{
//	//Set up the chassis mass, dimensions, moment of inertia, and center of mass offset.
//	//The moment of inertia is just the moment of inertia of a cuboid but modified for easier steering.
//	//Center of mass offset is 0.65m above the base of the chassis and 0.25m towards the front.
//	const PxF32 chassisMass = 1500.0f;
//	const PxVec3 chassisDims(2.5f, 2.0f, 5.0f);
//	const PxVec3 chassisMOI
//	((chassisDims.y * chassisDims.y + chassisDims.z * chassisDims.z) * chassisMass / 12.0f,
//		(chassisDims.x * chassisDims.x + chassisDims.z * chassisDims.z) * 0.8f * chassisMass / 12.0f,
//		(chassisDims.x * chassisDims.x + chassisDims.y * chassisDims.y) * chassisMass / 12.0f);
//	const PxVec3 chassisCMOffset(0.0f, -chassisDims.y * 0.5f + 0.65f, 0.25f);
//
//	//Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
//	//Moment of inertia is just the moment of inertia of a cylinder.
//	const PxF32 wheelMass = 20.0f;
//	const PxF32 wheelRadius = 0.5f;
//	const PxF32 wheelWidth = 0.4f;
//	const PxF32 wheelMOI = 0.5f * wheelMass * wheelRadius * wheelRadius;
//	const PxU32 nbWheels = 6;
//
//	VehicleDesc vehicleDesc;
//
//	vehicleDesc.chassisMass = chassisMass;
//	vehicleDesc.chassisDims = chassisDims;
//	vehicleDesc.chassisMOI = chassisMOI;
//	vehicleDesc.chassisCMOffset = chassisCMOffset;
//	vehicleDesc.chassisMaterial = gMaterial;
//	vehicleDesc.chassisSimFilterData = PxFilterData(COLLISION_FLAG_CHASSIS, COLLISION_FLAG_CHASSIS_AGAINST, 0, 0);
//
//	vehicleDesc.wheelMass = wheelMass;
//	vehicleDesc.wheelRadius = wheelRadius;
//	vehicleDesc.wheelWidth = wheelWidth;
//	vehicleDesc.wheelMOI = wheelMOI;
//	vehicleDesc.numWheels = nbWheels;
//	vehicleDesc.wheelMaterial = gMaterial;
//	vehicleDesc.chassisSimFilterData = PxFilterData(COLLISION_FLAG_WHEEL, COLLISION_FLAG_WHEEL_AGAINST, 0, 0);
//
//	return vehicleDesc;
//}
//
//void startAccelerateForwardsMode()
//{
//	if (gMimicKeyInputs)
//	{
//		gVehicleInputData.setDigitalAccel(true);
//	}
//	else
//	{
//		gVehicleInputData.setAnalogAccel(1.0f);
//	}
//}
//
//void startAccelerateReverseMode()
//{
//	gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eREVERSE);
//
//	if (gMimicKeyInputs)
//	{
//		gVehicleInputData.setDigitalAccel(true);
//	}
//	else
//	{
//		gVehicleInputData.setAnalogAccel(1.0f);
//	}
//}
//
//void startBrakeMode()
//{
//	if (gMimicKeyInputs)
//	{
//		gVehicleInputData.setDigitalBrake(true);
//	}
//	else
//	{
//		gVehicleInputData.setAnalogBrake(1.0f);
//	}
//}
//
//void startTurnHardLeftMode()
//{
//	if (gMimicKeyInputs)
//	{
//		gVehicleInputData.setDigitalAccel(true);
//		gVehicleInputData.setDigitalSteerLeft(true);
//	}
//	else
//	{
//		gVehicleInputData.setAnalogAccel(true);
//		gVehicleInputData.setAnalogSteer(-1.0f);
//	}
//}
//
//void startTurnHardRightMode()
//{
//	if (gMimicKeyInputs)
//	{
//		gVehicleInputData.setDigitalAccel(true);
//		gVehicleInputData.setDigitalSteerRight(true);
//	}
//	else
//	{
//		gVehicleInputData.setAnalogAccel(1.0f);
//		gVehicleInputData.setAnalogSteer(1.0f);
//	}
//}
//
//void startHandbrakeTurnLeftMode()
//{
//	if (gMimicKeyInputs)
//	{
//		gVehicleInputData.setDigitalSteerLeft(true);
//		gVehicleInputData.setDigitalHandbrake(true);
//	}
//	else
//	{
//		gVehicleInputData.setAnalogSteer(-1.0f);
//		gVehicleInputData.setAnalogHandbrake(1.0f);
//	}
//}
//
//void startHandbrakeTurnRightMode()
//{
//	if (gMimicKeyInputs)
//	{
//		gVehicleInputData.setDigitalSteerRight(true);
//		gVehicleInputData.setDigitalHandbrake(true);
//	}
//	else
//	{
//		gVehicleInputData.setAnalogSteer(1.0f);
//		gVehicleInputData.setAnalogHandbrake(1.0f);
//	}
//}
//
//
//void releaseAllControls()
//{
//	if (gMimicKeyInputs)
//	{
//		gVehicleInputData.setDigitalAccel(false);
//		gVehicleInputData.setDigitalSteerLeft(false);
//		gVehicleInputData.setDigitalSteerRight(false);
//		gVehicleInputData.setDigitalBrake(false);
//		gVehicleInputData.setDigitalHandbrake(false);
//	}
//	else
//	{
//		gVehicleInputData.setAnalogAccel(0.0f);
//		gVehicleInputData.setAnalogSteer(0.0f);
//		gVehicleInputData.setAnalogBrake(0.0f);
//		gVehicleInputData.setAnalogHandbrake(0.0f);
//	}
//}
//
//void initPhysics()
//{
//	// EVERYTHING BEFORE THIS HAS ALREADY BEEN IMPLEMENTED AND JUST DELETED
//	//Create a plane to drive on.
//	PxFilterData groundPlaneSimFilterData(COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST, 0, 0);
//	gGroundPlane = createDrivablePlane(groundPlaneSimFilterData, gMaterial, gPhysics);
//	gScene->addActor(*gGroundPlane);
//
//	//Create a vehicle that will drive on the plane.
//	VehicleDesc vehicleDesc = initVehicleDesc();
//	gVehicle4W = createVehicle4W(vehicleDesc, gPhysics, gCooking);
//	PxTransform startTransform(PxVec3(0, (vehicleDesc.chassisDims.y * 0.5f + vehicleDesc.wheelRadius + 1.0f), 0), PxQuat(PxIdentity));
//	gVehicle4W->getRigidDynamicActor()->setGlobalPose(startTransform);
//	gScene->addActor(*gVehicle4W->getRigidDynamicActor());
//
//	//Set the vehicle to rest in first gear.
//	//Set the vehicle to use auto-gears.
//	gVehicle4W->setToRestState();
//	gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
//	gVehicle4W->mDriveDynData.setUseAutoGears(true);
//
//	gVehicleModeTimer = 0.0f;
//	gVehicleOrderProgress = 0;
//	startBrakeMode();
//}
//
//void incrementDrivingMode(const PxF32 timestep)
//{
//	gVehicleModeTimer += timestep;
//	if (gVehicleModeTimer > gVehicleModeLifetime)
//	{
//		//If the mode just completed was eDRIVE_MODE_ACCEL_REVERSE then switch back to forward gears.
//		if (eDRIVE_MODE_ACCEL_REVERSE == gDriveModeOrder[gVehicleOrderProgress])
//		{
//			gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
//		}
//
//		//Increment to next driving mode.
//		gVehicleModeTimer = 0.0f;
//		gVehicleOrderProgress++;
//		releaseAllControls();
//
//		//If we are at the end of the list of driving modes then start again.
//		if (eDRIVE_MODE_NONE == gDriveModeOrder[gVehicleOrderProgress])
//		{
//			gVehicleOrderProgress = 0;
//			gVehicleOrderComplete = true;
//		}
//
//		//Start driving in the selected mode.
//		DriveMode eDriveMode = gDriveModeOrder[gVehicleOrderProgress];
//		switch (eDriveMode)
//		{
//		case eDRIVE_MODE_ACCEL_FORWARDS:
//			startAccelerateForwardsMode();
//			break;
//		case eDRIVE_MODE_ACCEL_REVERSE:
//			startAccelerateReverseMode();
//			break;
//		case eDRIVE_MODE_HARD_TURN_LEFT:
//			startTurnHardLeftMode();
//			break;
//		case eDRIVE_MODE_HANDBRAKE_TURN_LEFT:
//			startHandbrakeTurnLeftMode();
//			break;
//		case eDRIVE_MODE_HARD_TURN_RIGHT:
//			startTurnHardRightMode();
//			break;
//		case eDRIVE_MODE_HANDBRAKE_TURN_RIGHT:
//			startHandbrakeTurnRightMode();
//			break;
//		case eDRIVE_MODE_BRAKE:
//			startBrakeMode();
//			break;
//		case eDRIVE_MODE_NONE:
//			break;
//		};
//
//		//If the mode about to start is eDRIVE_MODE_ACCEL_REVERSE then switch to reverse gears.
//		if (eDRIVE_MODE_ACCEL_REVERSE == gDriveModeOrder[gVehicleOrderProgress])
//		{
//			gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eREVERSE);
//		}
//	}
//}
//
//void stepPhysics()
//{
//	const PxF32 timestep = 1.0f / 60.0f;
//
//	//Cycle through the driving modes to demonstrate how to accelerate/reverse/brake/turn etc.
//	incrementDrivingMode(timestep);
//
//	//Update the control inputs for the vehicle.
//	if (gMimicKeyInputs)
//	{
//		PxVehicleDrive4WSmoothDigitalRawInputsAndSetAnalogInputs(gKeySmoothingData, gSteerVsForwardSpeedTable, gVehicleInputData, timestep, gIsVehicleInAir, *gVehicle4W);
//	}
//	else
//	{
//		PxVehicleDrive4WSmoothAnalogRawInputsAndSetAnalogInputs(gPadSmoothingData, gSteerVsForwardSpeedTable, gVehicleInputData, timestep, gIsVehicleInAir, *gVehicle4W);
//	}
//
//	//Raycasts.
//	PxVehicleWheels* vehicles[1] = { gVehicle4W };
//	PxRaycastQueryResult* raycastResults = gVehicleSceneQueryData->getRaycastQueryResultBuffer(0);
//	const PxU32 raycastResultsSize = gVehicleSceneQueryData->getQueryResultBufferSize();
//	PxVehicleSuspensionRaycasts(gBatchQuery, 1, vehicles, raycastResultsSize, raycastResults);
//
//	//Vehicle update.
//	const PxVec3 grav = gScene->getGravity();
//	PxWheelQueryResult wheelQueryResults[PX_MAX_NB_WHEELS];
//	PxVehicleWheelQueryResult vehicleQueryResults[1] = { {wheelQueryResults, gVehicle4W->mWheelsSimData.getNbWheels()} };
//	PxVehicleUpdates(timestep, grav, *gFrictionPairs, 1, vehicles, vehicleQueryResults);
//
//	//Work out if the vehicle is in the air.
//	gIsVehicleInAir = gVehicle4W->getRigidDynamicActor()->isSleeping() ? false : PxVehicleIsInAir(vehicleQueryResults[0]);
//
//	//Scene update.
//	gScene->simulate(timestep);
//	gScene->fetchResults(true);
//}
//
//void cleanupPhysics()
//{
//	gVehicle4W->getRigidDynamicActor()->release();
//	gVehicle4W->free();
//	PX_RELEASE(gGroundPlane);
//	PX_RELEASE(gBatchQuery);
//	gVehicleSceneQueryData->free(gAllocator);
//	PX_RELEASE(gFrictionPairs);
//	PxCloseVehicleSDK();
//
//	PX_RELEASE(gMaterial);
//	PX_RELEASE(gCooking);
//	PX_RELEASE(gScene);
//	PX_RELEASE(gDispatcher);
//	PX_RELEASE(gPhysics);
//	if (gPvd)
//	{
//		PxPvdTransport* transport = gPvd->getTransport();
//		gPvd->release();	gPvd = NULL;
//		PX_RELEASE(transport);
//	}
//	PX_RELEASE(gFoundation);
//
//	printf("SnippetVehicle4W done.\n");
//}
//
//void keyPress(unsigned char key, const PxTransform& camera)
//{
//	PX_UNUSED(camera);
//	PX_UNUSED(key);
//}
//
//int snippetMain(int, const char* const*)
//{
//	initPhysics();
//	while (!gVehicleOrderComplete)
//	{
//		stepPhysics();
//	}
//	cleanupPhysics();
//#endif
//
//	return 0;
//}
