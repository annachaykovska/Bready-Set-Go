#pragma once
#include <unordered_map>

#include "Component.h"
#include "../Transform.h"
#include "../Audio/AudioSource.h"
#include <PxPhysicsAPI.h>
#include "../Rendering/Model.h"
#include <vector>
#include "../Inventory.h"

class Entity {

public:
	std::string name;
	physx::PxVehicleDrive4W* vehicle;
	physx::PxTransform originalSpawn;
	bool bannerSoundPlayed;
	
	Entity(std::string name);
	bool attachComponent(Component* newComponent, std::string);
	//bool attachComponent(Component* newComponent, std::string&);
	Component* getComponent(std::string);
	Transform* getTransform();
	Model* getModel();
	Inventory* getInventory();
	AudioSource* getAudioSource();


	inline bool IngredientCanBeCollected(std::string name);
	std::vector<Entity*> getCollectableIngredients();
	std::vector<std::string> getOtherPlayers(); 
	void checkIngredientCollision(Entity* otherEntity);
	void checkPlayerCollision(Entity* otherEntity);

	bool removeFlag;
	float speed;
	glm::vec3 orientation;

	bool verifyPlayerCollision;
	std::string otherPlayerInCollision;
	float collisionCooldownStart;

	// Grab ability variables
	int magnetStatus;
	float magnetDistanceSquared; // Max distance that can be stolen from (squared for faster calculations)
	float magnetCooldown;
	float lastMagnetUse;
	float unflipTimer;

	// Time period when player cannot be stolen from
	float gracePeriod;
	float lastGracePeriodStart;

	// For the tethered vacuum
	bool tethered;
	std::vector<Entity*> tethered_victims;
	float magnetTimeToSteal;
	float magnetStartTime;

	// For ingredient collection
	float ingredientCollectDistanceSquared;

private:

	std::unordered_map<std::string, Component*> components;
};
