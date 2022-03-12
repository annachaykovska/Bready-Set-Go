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
        
        if (entity1->name == "tomato" && playerInventory->tomato == 0)
        {
            playerInventory->tomato = 1;
            entity0->getAudioSource()->play("pickup.wav");
            //entity1->getModel()->visible = false;
            entity1->removeFlag = true;
        }
        if (entity1->name == "cheese" && playerInventory->cheese == 0)
        {
            playerInventory->cheese = 1;
            entity0->getAudioSource()->play("pickup.wav");
            //entity1->getModel()->visible = false;
            entity1->removeFlag = true;
        }
        if (entity1->name == "dough" && playerInventory->dough == 0)
        {
            playerInventory->dough = 1;
            entity0->getAudioSource()->play("pickup.wav");
            //entity1->getModel()->visible = false;
            entity1->removeFlag = true;
        }
        if (entity1->name == "sausage" && playerInventory->sausage == 0) {
            playerInventory->sausage = 1;
            entity0->getAudioSource()->play("pickup.wav");
            //entity1->getModel()->visible = false;
            entity1->removeFlag = true;
        }

        if (playerInventory->checkPizza())
            entity0->getAudioSource()->play("ding.wav");

        // OTHER PLAYERS
        vector<string> otherPlayers = entity0->getOtherPlayers();
        if (entity1->name == otherPlayers[0] || entity1->name == otherPlayers[1] || entity1->name == otherPlayers[2])
        {
            entity0->getAudioSource()->play("thump.wav");
            Inventory* opponentInv = (Inventory*)entity1->getComponent("inventory");
            int ingredient = playerInventory->removeRandomPizzaIngredient(opponentInv->cheese, opponentInv->dough, opponentInv->sausage, opponentInv->tomato);
            opponentInv->setIngredientFromId(ingredient);
            //printf("%s collided with %s and transferred food id %d\n", entity0->name.c_str(), entity1->name.c_str(), ingredient);
        }
    }
    else if (entity1->name == "player1" || entity1->name == "player2" || entity1->name == "player3" || entity1->name == "player4")
    {
        Inventory* playerInventory = (Inventory*)entity1->getComponent("inventory");

        // FOOD 
        if (entity0->name == "tomato" && playerInventory->tomato == 0)
        {
            playerInventory->tomato = 1;
            entity1->getAudioSource()->play("pickup.wav");
            //entity0->getModel()->visible = false;
            entity0->removeFlag = true;
        }
        if (entity0->name == "cheese" && playerInventory->cheese == 0)
        {
            playerInventory->cheese = 1;
            entity1->getAudioSource()->play("pickup.wav");
            //entity0->getModel()->visible = false;
            entity0->removeFlag = true;
        }
        if (entity0->name == "dough" && playerInventory->dough == 0)
        {
            playerInventory->dough = 1;
            entity1->getAudioSource()->play("pickup.wav");
            //entity0->getModel()->visible = false;
            entity0->removeFlag = true;
        }
        if (entity0->name == "sausage" && playerInventory->sausage == 0)
        {
            playerInventory->sausage = 1;
            entity1->getAudioSource()->play("pickup.wav");
            //entity0->getModel()->visible = false;
            entity0->removeFlag = true;
        }
        if (playerInventory->checkPizza())
            entity1->getAudioSource()->play("ding.wav");

        // OTHER PLAYERS
        vector<string> otherPlayers = entity1->getOtherPlayers();
        if (entity0->name == otherPlayers[0] || entity0->name == otherPlayers[1] || entity0->name == otherPlayers[2])
        {
            entity1->getAudioSource()->play("thump.wav");
            Inventory* opponentInv = (Inventory*)entity0->getComponent("inventory");
            int ingredient = playerInventory->removeRandomPizzaIngredient(opponentInv->cheese, opponentInv->dough, opponentInv->sausage, opponentInv->tomato);
            opponentInv->setIngredientFromId(ingredient);
            //printf("%s collided with %s and transferred food id %d\n", entity0->name.c_str(), entity1->name.c_str(), ingredient);
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