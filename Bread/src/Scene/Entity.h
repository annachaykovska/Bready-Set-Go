#pragma once
#include <unordered_map>

#include "Component.h"
#include "../Transform.h"
#include "../Audio/AudioSource.h"
#include <PxPhysicsAPI.h>
#include "../Rendering/Model.h"
#include <vector>

class Entity {

public:
	std::string name;
	physx::PxVehicleDrive4W* vehicle;
	
	Entity(std::string name);
	Entity();
	bool attachComponent(Component* newComponent, std::string);
	//bool attachComponent(Component* newComponent, std::string&);
	Component* getComponent(std::string);
	Transform* getTransform();
	Model* getModel();
	AudioSource* getAudioSource();
	std::vector<std::string> getOtherPlayers();

	bool removeFlag;
	float speed;
	glm::vec3 orientation;

private:

	std::unordered_map<std::string, Component*> components;
};
