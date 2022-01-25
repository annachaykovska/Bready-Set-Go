#include "Scene.h"

/// <summary>
/// Creates a new Entity in the Scene.
/// </summary>
/// <returns>
/// Returns a pointer to the Entity if successful. Nullptr otherwise.
/// </returns>
Entity* Scene::createEntity(std::string name)
{
	// Add new entity to map
	entities.emplace(name, Entity(name));
	
	// Check to make sure entity was added successfully
	auto result = entities.find(name);
	if (result != entities.end())
		return &result->second;
	else
		return nullptr;
}

Entity* Scene::getEntity(std::string name)
{
	return nullptr;
}