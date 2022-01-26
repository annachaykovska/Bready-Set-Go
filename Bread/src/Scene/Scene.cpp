#include "Scene.h"


/// <summary>
/// Creates all the Entities in the Scene.
/// </summary>
void Scene::createEntities()
{
	createEntity("player1");
	createEntity("player2");
	createEntity("countertop");
}


/// <summary>
/// Returns a reference to the Entity with name.
/// </summary>
/// <param name="name">
/// The name of the Entity.
/// </param>
/// <returns>
/// Returns a pointer to the Entity if successful. Nullptr otherwise.
/// </returns>
Entity* Scene::getEntity(std::string name)
{
	auto result = entities.find(name);

	if (result != entities.end())
		return &(result->second);
	else
		return nullptr;
}

/// <summary>
/// Returns the number of Entities in the Scene.
/// </summary>
/// <returns>
/// int number of Entities.
/// </returns>
int Scene::count()
{
	size_t count = entities.size();

	return (int)count * 2;
}

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