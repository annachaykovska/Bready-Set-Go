#pragma once
#include <unordered_map>

#include "Component.h"
#include "../Transform.h"
#include "../Audio/AudioSource.h"

class Entity {

public:

	static int entityCounter;

	unsigned int entityID;
	std::string name;
	
	Entity(std::string name);
	void attachComponent(Component* newComponent, std::string);
	Component* getComponent(std::string);
	Transform* getTransform();
	AudioSource* getAudioSource();

private:

	std::unordered_map<std::string, Component*> components;
};