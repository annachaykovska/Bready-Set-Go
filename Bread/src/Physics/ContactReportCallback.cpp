

class ContactReportCallback : public PxSimulationEventCallback {
    virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
    {
        PxRigidActor* actor0 = pairHeader.actors[0];
        PxRigidActor* actor1 = pairHeader.actors[1];
        printf("YEET");
    }

    virtual void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) {
        PX_UNUSED(constraints);
        PX_UNUSED(count);
    }

    virtual void onWake(PxActor** actors, PxU32 count) {
        PX_UNUSED(actors);
        PX_UNUSED(count);
    }

    virtual void onSleep(PxActor** actors, PxU32 count) {
        PX_UNUSED(actors);
        PX_UNUSED(count);
    }

    virtual void onTrigger(PxTriggerPair* pairs, PxU32 count) {
        PX_UNUSED(pairs);
        PX_UNUSED(count);
    }

    virtual void onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) {
        PX_UNUSED(bodyBuffer);
        PX_UNUSED(poseBuffer);
        PX_UNUSED(count);
    }
};