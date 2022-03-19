#include <stdio.h>
#include <iostream>

#include <PxPhysicsAPI.h>

#include "CollisionCallback.h"
#include "../Scene/Scene.h"
#include "../Scene/Entity.h"
#include "../Inventory.h"

using namespace std;

extern Scene g_scene;


void CollisionCallback::onContact(const physx::PxContactPairHeader& pairHeader, 
                                  const physx::PxContactPair* pairs, 
                                        physx::PxU32 nbPairs)
{
    // Get PxRigidActors from the collision pair
    physx::PxRigidActor* actor0 = pairHeader.actors[0];
    physx::PxRigidActor* actor1 = pairHeader.actors[1];

    // Convert the actor's userData to their names as strings
    std::string a0name = *(static_cast<std::string*>(actor0->userData));
    std::string a1name = *(static_cast<std::string*>(actor1->userData));

    // Use the name strings to find the Entities
    Entity* entity0 = g_scene.getEntity(a0name);
    Entity* entity1 = g_scene.getEntity(a1name);

    if (entity0->name == "player1" || entity0->name == "player2" || entity0->name == "player3" || entity0->name == "player4")
    {
        Inventory* playerInventory = (Inventory*)entity0->getComponent("inventory");
        // FOOD
        entity0->checkIngredientCollision(entity1);
        // OTHER PLAYERS
        entity0->checkPlayerCollision(entity1);
    }
    else if (entity1->name == "player1" || entity1->name == "player2" || entity1->name == "player3" || entity1->name == "player4")
    {
        Inventory* playerInventory = (Inventory*)entity1->getComponent("inventory");
        // FOOD 
        entity1->checkIngredientCollision(entity0);
        // OTHER PLAYERS
        entity1->checkPlayerCollision(entity0);
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
