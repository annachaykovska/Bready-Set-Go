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
    // Get PxRigidActors from the collision pair
    physx::PxRigidActor* actor0 = pairHeader.actors[0];
    physx::PxRigidActor* actor1 = pairHeader.actors[1];

    // Convert the actor's userData to their names as strings
    std::string a0name = *(static_cast<std::string*>(actor0->userData));
    std::string a1name = *(static_cast<std::string*>(actor1->userData));

    // Use the name strings to find the Entities
    Entity* entity0 = g_scene.getEntity(a0name);
    Entity* entity1 = g_scene.getEntity(a1name);

    //std::cout << "entity0->name = " << entity0->name << std::endl;
    //std::cout << "entity1->name = " << entity1->name << std::endl;

    // TODO make this better and part of GameLogic in the future
    if (entity0->name == "player1")
    {
        Inventory* p1Inv = (Inventory*)entity0->getComponent("inventory");
        
        if (entity1->name == "tomato" && p1Inv->tomato == 0)
        {
            p1Inv->tomato = 1;
            entity0->getAudioSource()->play("pickup.wav");
            entity1->getModel()->visible = false;
        }
        if (entity1->name == "cheese" && p1Inv->cheese == 0)
        {
            p1Inv->cheese = 1;
            entity0->getAudioSource()->play("pickup.wav");
            entity1->getModel()->visible = false;
        }
        if (entity1->name == "dough" && p1Inv->dough == 0) 
        {
            p1Inv->dough = 1;
            entity0->getAudioSource()->play("pickup.wav");
            entity1->getModel()->visible = false;
        }
        if (entity1->name == "sausage" && p1Inv->sausage == 0) {
            p1Inv->sausage = 1;
            entity0->getAudioSource()->play("pickup.wav");
            entity1->getModel()->visible = false;
        }

        if (p1Inv->checkPizza())
            entity0->getAudioSource()->play("ding.wav");

        // OTHER PLAYERS
        if (entity1->name == "player2" || entity1->name == "player3" || entity1->name == "player4")
        {
            entity0->getAudioSource()->play("thump.wav");
            printf("%s collided with %s\n", entity0->name.c_str(), entity1->name.c_str());
        }
    }
    else if (entity1->name == "player1")
    {
        Inventory* p1Inv = (Inventory*)entity1->getComponent("inventory");

        // FOOD 
        if (entity0->name == "tomato" && p1Inv->tomato == 0)
        {
            p1Inv->tomato = 1;
            entity1->getAudioSource()->play("pickup.wav");
            entity0->getModel()->visible = false;
        }
        if (entity0->name == "cheese" && p1Inv->cheese == 0)
        {
            p1Inv->cheese = 1;
            entity1->getAudioSource()->play("pickup.wav");
            entity0->getModel()->visible = false;
        }
        if (entity0->name == "dough" && p1Inv->dough == 0)
        {
            p1Inv->dough = 1;
            entity1->getAudioSource()->play("pickup.wav");
            entity0->getModel()->visible = false;
        }
        if (entity0->name == "sausage" && p1Inv->sausage == 0)
        {
            p1Inv->sausage = 1;
            entity1->getAudioSource()->play("pickup.wav");
            entity0->getModel()->visible = false;
        }
        if (p1Inv->checkPizza())
            entity1->getAudioSource()->play("ding.wav");

        // OTHER PLAYERS
        if (entity0->name == "player2" || entity0->name == "player3" || entity0->name == "player4")
        {
            entity1->getAudioSource()->play("thump.wav");
            printf("%s collided with %s\n", entity1->name.c_str(), entity0->name.c_str());
        }
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