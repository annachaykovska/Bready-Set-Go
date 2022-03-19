#pragma once
#include <unordered_map>

#include "../Camera.h"
#include "../Physics/PhysicsSystem.h"

class Entity;

class Scene
{
public:

	Camera camera;

	void createEntities();
	Entity* getEntity(std::string name);
	int count();

	void init(PhysicsSystem* physics);
	
private:

	Entity* createEntity(std::string name);
	std::unordered_map<std::string, Entity> entities;

};