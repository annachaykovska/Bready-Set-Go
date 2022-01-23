#pragma once
#include <vector>
#include <memory>

#include "Component.h"

static unsigned int entityCounter = 0;

class Entity {

public:

	unsigned int entityID;
	std::vector<std::shared_ptr<Component>> components;

	Entity();
	void addComponent(std::shared_ptr<Component> newComponent);

private:
};