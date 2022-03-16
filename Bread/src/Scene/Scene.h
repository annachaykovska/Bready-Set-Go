#pragma once
#include <unordered_map>

#include "../Camera.h"
#include "../Physics/PhysicsSystem.h"
#include "../Navigation/IngredientTracker.h"

class Entity;

class Scene
{
public:

	Camera camera;
	IngredientTracker* tracker;

	void createEntities();
	Entity* getEntity(std::string name);
	int count();

	void init(PhysicsSystem* physics);
	void initIngredientTracking(IngredientTracker* ingredientTracker);
	
private:

	Entity* createEntity(std::string name);
	std::unordered_map<std::string, Entity> entities;

};