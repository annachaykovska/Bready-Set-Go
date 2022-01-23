#include "Entity.h"

Entity::Entity()
{
	this->entityID = entityCounter++;
}

void Entity::addComponent(std::shared_ptr<Component> newComponent)
{
	newComponent->entityID = this->entityID;
	std::shared_ptr<Component> ptr(newComponent);
	this->components.push_back(ptr);
}