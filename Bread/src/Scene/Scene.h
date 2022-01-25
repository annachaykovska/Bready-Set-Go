#pragma once
#include <unordered_map>

#include "Entity.h"

class Scene
{
public:

	Entity* createEntity(std::string name);
	Entity* getEntity(std::string name);
	
private:

	std::unordered_map<std::string, Entity> entities;

};