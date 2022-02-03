#pragma once
#include <unordered_map>

#include "Component.h"
#include "../Transform.h"
#include "../Audio/AudioSource.h"
#include <PxPhysicsAPI.h>

using namespace physx;

class Entity {

public:
	std::string name;
	PxVehicleDrive4W* vehicle;
	
	Entity(std::string name);
	bool attachComponent(Component* newComponent, std::string);
	bool attachComponent(Component* newComponent, std::string&);
	Component* getComponent(std::string);
	Transform* getTransform();
	AudioSource* getAudioSource();

private:

	std::unordered_map<std::string, Component*> components;
};