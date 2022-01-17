#pragma once

class PhysicsSystem
{
public:

	PhysicsSystem();
	void update(const float timeStep);

private:

	physx::PxDefaultErrorCallback mDefaultErrorCallback;
	physx::PxDefaultAllocator mDefaultAllocatorCallback;
	physx::PxFoundation* mFoundation;
	physx::PxPhysics* mPhysics;
	physx::PxDefaultCpuDispatcher* mDispatcher;
	physx::PxScene* mScene;
	physx::PxCooking* gCooking;
	physx::PxMaterial* mMaterial;
	physx::PxPvd* mPvd;

};