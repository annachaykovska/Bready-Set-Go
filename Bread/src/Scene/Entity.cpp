#include "Entity.h"

Entity::Entity(std::string name)
{
	this->name = name;
	this->removeFlag = false;
}

Entity::Entity(){
	this->name = "default";
	this->removeFlag = false;
}

bool Entity::attachComponent(Component* newComponent, std::string name)
{
	if ((components.emplace(name, newComponent)).second)
	{
		newComponent->owner = this;
		return true;
	}
	else
		return false;
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

Model* Entity::getModel()
{
	Model* result = (Model*)(this->getComponent("model"));
	
	if (!result)
		std::cout << "Model component not found!\n";
	
	return result;
}

AudioSource* Entity::getAudioSource()
{
	return (AudioSource*)(this->getComponent("audio"));
}
