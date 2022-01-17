#include "Entity.h"

Entity::Entity()
{
	this->entityID = entityCounter++;
}

void Entity::addComponent(std::shared_ptr<Component> newComponent)
{
	newComponent->entityID = this->entityID;
	std::weak_ptr<Component> weakPtr(newComponent);
	this->components.push_back(weakPtr);
}