#pragma once
#include <unordered_map>

#include "Component.h"
#include "../Transform.h"
#include "../Audio/AudioSource.h"
#include "../Rendering/Model.h"

class Entity {

public:

	std::string name;
	
	Entity(std::string name);
	bool attachComponent(Component* newComponent, std::string);
	bool attachComponent(Component* newComponent, std::string&);
	Component* getComponent(std::string);
	Transform* getTransform();
	Model* getModel();
	AudioSource* getAudioSource();

private:

	std::unordered_map<std::string, Component*> components;
};