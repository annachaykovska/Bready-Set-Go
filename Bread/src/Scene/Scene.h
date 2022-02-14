#pragma once
#include <unordered_map>

#include "../Camera.h"

class Entity;

class Scene
{
public:

	Camera camera;

	void createEntities();
	Entity* getEntity(std::string name);
	int count();
	
private:

	Entity* createEntity(std::string name);
	std::unordered_map<std::string, Entity> entities;

};