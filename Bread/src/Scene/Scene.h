#pragma once
#include <unordered_map>

#include "../Camera.h"
#include "../Physics/PhysicsSystem.h"

class Entity;

class Scene
{
public:

	Scene();
	~Scene();

	Camera* p1Camera;
	Camera* p2Camera;
	Camera* p3Camera;
	Camera* p4Camera;

	void createEntities();
	Entity* getEntity(std::string name);
	int entityCount();
	std::vector<Entity*> getIngredients();
	std::vector<Entity*> getPlayers();

	int numPlayers;
	
private:

	Entity* createEntity(std::string name);
	std::unordered_map<std::string, Entity> entities;

	std::vector<Entity*> ingredients;
	std::vector<Entity*> players;
};