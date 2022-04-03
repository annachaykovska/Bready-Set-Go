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
	
	Entity(std::string name);
	Entity();
	bool attachComponent(Component* newComponent, std::string);
	//bool attachComponent(Component* newComponent, std::string&);
	Component* getComponent(std::string);
	Transform* getTransform();
	Model* getModel();
	Inventory* getInventory();
	AudioSource* getAudioSource();
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
	float magnetDistanceSquared;

	float magnetCooldown;
	float lastMagnetUse; // Time at which the magnet ability was used last

	float stolenFromGracePeriod;
	float lastStolenFrom;

private:

	std::unordered_map<std::string, Component*> components;
};
