#include <stdio.h>
#include <iostream>

#include <PxPhysicsAPI.h>

#include "CollisionCallback.h"
#include "../Scene/Scene.h"
#include "../Scene/Entity.h"
#include "../Inventory.h"


extern Scene g_scene;

void CollisionCallback::onContact(const physx::PxContactPairHeader& pairHeader, 
                                      const physx::PxContactPair* pairs, 
                                            physx::PxU32 nbPairs)
{
    physx::PxRigidActor* actor0 = pairHeader.actors[0];
    physx::PxRigidActor* actor1 = pairHeader.actors[1];

    std::string a0name = *(static_cast<std::string*>(actor0->userData));
    std::string a1name = *(static_cast<std::string*>(actor1->userData));

    // TODO remove debug code
    //std::cout << "actor0 = " << a0name << std::endl;
    //std::cout << "actor1 = " << a1name << std::endl;

    Entity* entity0 = g_scene.getEntity(a0name);
    Entity* entity1 = g_scene.getEntity(a1name);

    std::cout << "entity0->name = " << entity0->name << std::endl;
    std::cout << "entity1->name = " << entity1->name << std::endl;

    if (entity0->name == "player1")
    {
        std::cout << "here" << std::endl;

        Inventory* p1Inv = (Inventory*)entity0->getComponent("inventory");
        
        if (entity1->name == "tomato")
            p1Inv->tomato = 1;
        if (entity1->name == "cheese")
            p1Inv->cheese = 1;
        if (entity1->name == "dough")
            p1Inv->dough = 1;
        if (entity1->name == "sausage")
            p1Inv->sausage = 1;
    }
    else if (entity1->name == "player1")
    {
        std::cout << "there" << std::endl;

        Inventory* p1Inv = (Inventory*)entity1->getComponent("inventory");

        if (entity0->name == "tomato")
            p1Inv->tomato = 1;
        if (entity0->name == "cheese")
            p1Inv->cheese = 1;
        if (entity0->name == "dough")
            p1Inv->dough = 1;
        if (entity0->name == "sausage")
            p1Inv->sausage = 1;
    }
}

void CollisionCallback::onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count) 
{
    PX_UNUSED(constraints);
    PX_UNUSED(count);

    printf("onConstraintBreak\n");
}

void CollisionCallback::onWake(physx::PxActor** actors, physx::PxU32 count) 
{
    PX_UNUSED(actors);
    PX_UNUSED(count);

    printf("onWake\n");
}

void CollisionCallback::onSleep(physx::PxActor** actors, physx::PxU32 count) 
{
    PX_UNUSED(actors);
    PX_UNUSED(count);

    printf("onSleep\n");
}

void CollisionCallback::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) 
{
    PX_UNUSED(pairs);
    PX_UNUSED(count);

    printf("onTrigger\n");
}

void CollisionCallback::onAdvance(const physx::PxRigidBody* const* bodyBuffer, 
                                      const physx::PxTransform* poseBuffer, 
                                      const physx::PxU32 count) 
{
    PX_UNUSED(bodyBuffer);
    PX_UNUSED(poseBuffer);
    PX_UNUSED(count);

    printf("onAdvance\n");
}