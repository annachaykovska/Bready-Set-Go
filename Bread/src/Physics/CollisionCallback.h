#pragma once

#include "PxSimulationEventCallback.h"

class CollisionCallback : public physx::PxSimulationEventCallback 
{

public:

    void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs);
    void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count);
    void onWake(physx::PxActor** actors, physx::PxU32 count);
    void onSleep(physx::PxActor** actors, physx::PxU32 count);
    void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count);
    void onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count);
};