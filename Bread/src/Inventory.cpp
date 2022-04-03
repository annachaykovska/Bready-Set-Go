#include "Inventory.h"

#include "SystemManager.h"
#include "Scene/Scene.h"
#include "Scene/Entity.h"

extern Scene g_scene;
extern SystemManager g_systems;

void Inventory::clearAllIngredients()
{
	this->cheese = 0;
	this->dough = 0;
	this->sausage = 0;
	this->tomato = 0;
	this->carrot = 0;
	this->lettuce = 0;
	this->parsnip = 0;
	this->rice = 0;
	this->egg = 0;
	this->chicken = 0;
	this->peas = 0;
	//this->soupbase = 0;
	//this->pumpkin = 0;
}

// Removes a random ingredient from this player's inventory
void Inventory::clearRandomIngredient()
{
	int ingredientNumbers[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

	std::random_device r;
	std::seed_seq seed{ r(), r(), r(), r(), r(), r(), r(), r() };
	std::mt19937 eng(seed);

	// Not actually using this
	std::shuffle(std::begin(ingredientNumbers), std::end(ingredientNumbers), eng);

	for (int i = 0; i < 11; i++)
	{
		switch (ingredientNumbers[i])
		{
		case 0:
			if (this->cheese == 1)
			{
				this->cheese = 0;
				g_systems.physics->cheese->setGlobalPose(g_scene.getEntity("cheese")->originalSpawn);
				g_systems.tracker->updateCheeseTransformSource(g_scene.getEntity("cheese")->getTransform());
				g_systems.physics->cheese->setLinearVelocity(physx::PxVec3(0));
				g_systems.physics->cheese->setAngularVelocity(physx::PxVec3(0));
				g_scene.getEntity("cheese")->removeFlag = false;
				i = 11;
			}
			break;
		case 1:
			if (this->tomato == 1)
			{
				this->tomato = 0;
				g_systems.physics->tomato->setGlobalPose(g_scene.getEntity("tomato")->originalSpawn);
				g_systems.tracker->updateTomatoTransformSource(g_scene.getEntity("tomato")->getTransform());
				g_systems.physics->tomato->setLinearVelocity(physx::PxVec3(0));
				g_systems.physics->tomato->setAngularVelocity(physx::PxVec3(0));
				g_scene.getEntity("tomato")->removeFlag = false;
				i = 11;
			}
			break;
		case 2:
			if (this->sausage == 1)
			{
				this->sausage = 0;
				g_systems.physics->sausage->setGlobalPose(g_scene.getEntity("sausage")->originalSpawn);
				g_systems.tracker->updateSausageTransformSource(g_scene.getEntity("sausage")->getTransform());
				g_systems.physics->sausage->setLinearVelocity(physx::PxVec3(0));
				g_systems.physics->sausage->setAngularVelocity(physx::PxVec3(0));
				g_scene.getEntity("sausage")->removeFlag = false;
				i = 11;
			}
			break;
		case 3:
			if (this->dough == 1)
			{
				this->dough = 0;
				g_systems.physics->dough->setGlobalPose(g_scene.getEntity("dough")->originalSpawn);
				g_systems.tracker->updateDoughTransformSource(g_scene.getEntity("dough")->getTransform());
				g_systems.physics->dough->setLinearVelocity(physx::PxVec3(0));
				g_systems.physics->dough->setAngularVelocity(physx::PxVec3(0));
				g_scene.getEntity("dough")->removeFlag = false;
				i = 11;
			}
			break;
		case 4:
			if (this->carrot == 1)
			{
				this->carrot = 0;
				g_systems.physics->carrot->setGlobalPose(g_scene.getEntity("carrot")->originalSpawn);
				g_systems.tracker->updateCarrotTransformSource(g_scene.getEntity("carrot")->getTransform());
				g_systems.physics->carrot->setLinearVelocity(physx::PxVec3(0));
				g_systems.physics->carrot->setAngularVelocity(physx::PxVec3(0));
				g_scene.getEntity("carrot")->removeFlag = false;
				i = 11;
			}
			break;
		case 5:
			if (this->lettuce == 1)
			{
				this->lettuce = 0;
				g_systems.physics->lettuce->setGlobalPose(g_scene.getEntity("lettuce")->originalSpawn);
				g_systems.tracker->updateLettuceTransformSource(g_scene.getEntity("lettuce")->getTransform());
				g_systems.physics->lettuce->setLinearVelocity(physx::PxVec3(0));
				g_systems.physics->lettuce->setAngularVelocity(physx::PxVec3(0));
				g_scene.getEntity("lettuce")->removeFlag = false;
				i = 11;
			}
			break;
		case 6:
			if (this->parsnip == 1)
			{
				this->parsnip = 0;
				g_systems.physics->parsnip->setGlobalPose(g_scene.getEntity("parsnip")->originalSpawn);
				g_systems.tracker->updateParsnipTransformSource(g_scene.getEntity("parsnip")->getTransform());
				g_systems.physics->parsnip->setLinearVelocity(physx::PxVec3(0));
				g_systems.physics->parsnip->setAngularVelocity(physx::PxVec3(0));
				g_scene.getEntity("parsnip")->removeFlag = false;
				i = 11;
			}
			break;
		case 7:
			if (this->rice == 1)
			{
				this->rice = 0;
				g_systems.physics->rice->setGlobalPose(g_scene.getEntity("rice")->originalSpawn);
				g_systems.tracker->updateRiceTransformSource(g_scene.getEntity("rice")->getTransform());
				g_systems.physics->rice->setLinearVelocity(physx::PxVec3(0));
				g_systems.physics->rice->setAngularVelocity(physx::PxVec3(0));
				g_scene.getEntity("rice")->removeFlag = false;
				i = 11;
			}
			break;
		case 8:
			if (this->egg == 1)
			{
				this->egg = 0;
				g_systems.physics->egg->setGlobalPose(g_scene.getEntity("egg")->originalSpawn);
				g_systems.tracker->updateEggTransformSource(g_scene.getEntity("egg")->getTransform());
				g_systems.physics->egg->setLinearVelocity(physx::PxVec3(0));
				g_systems.physics->egg->setAngularVelocity(physx::PxVec3(0));
				g_scene.getEntity("egg")->removeFlag = false;
				i = 11;
			}
			break;
		case 9:
			if (this->chicken == 1)
			{
				this->chicken = 0;
				g_systems.physics->chicken->setGlobalPose(g_scene.getEntity("chicken")->originalSpawn);
				g_systems.tracker->updateChickenTransformSource(g_scene.getEntity("chicken")->getTransform());
				g_systems.physics->chicken->setLinearVelocity(physx::PxVec3(0));
				g_systems.physics->chicken->setAngularVelocity(physx::PxVec3(0));
				g_scene.getEntity("chicken")->removeFlag = false;
				i = 11;
			}
			break;
		case 10:
			if (this->peas == 1)
			{
				this->peas = 0;
				g_systems.physics->peas->setGlobalPose(g_scene.getEntity("peas")->originalSpawn);
				g_systems.tracker->updatePeasTransformSource(g_scene.getEntity("peas")->getTransform());
				g_systems.physics->peas->setLinearVelocity(physx::PxVec3(0));
				g_systems.physics->peas->setAngularVelocity(physx::PxVec3(0));
				g_scene.getEntity("peas")->removeFlag = false;
				i = 11;
			}
			break;
		}
	}
}