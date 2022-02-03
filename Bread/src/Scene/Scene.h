#pragma once
#include <unordered_map>

class Entity;

class Scene
{
public:

	void createEntities();
	Entity* getEntity(std::string name);
	int count();
	
private:

	Entity* createEntity(std::string name);
	std::unordered_map<std::string, Entity> entities;

};