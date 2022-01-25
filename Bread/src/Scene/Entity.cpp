#include "Entity.h"

int Entity::entityCounter;

Entity::Entity(std::string name)
{
	this->entityID = entityCounter++;
}

void Entity::attachComponent(Component* newComponent, std::string name)
{
	components.emplace(name, newComponent);
}

Component* Entity::getComponent(std::string key)
{
	auto result = components.find(key);

	if (result != components.end())
		return result->second;
	else
		return nullptr;
}

Transform* Entity::getTransform()
{
	return (Transform*)(this->getComponent("transform"));
}

AudioSource* Entity::getAudioSource()
{
	return (AudioSource*)(this->getComponent("audio"));
}