#include "Scene.h"
#include "Entity.h"

/// <summary>
/// Creates all the Entities in the Scene.
/// </summary>
void Scene::createEntities()
{
	// Game Entities
	// Players
	createEntity("player1");
	createEntity("player2");
	createEntity("player3");
	createEntity("player4");

	// Ingredients
	createEntity("cheese");
	createEntity("sausage");
	createEntity("tomato");
	createEntity("dough");
	createEntity("carrot");
	createEntity("lettuce");
	createEntity("parsnip");
	createEntity("rice");
	createEntity("egg");
	createEntity("chicken");
	createEntity("peas");
	createEntity("soupbase");
	createEntity("pumpkin");

	// World
	createEntity("countertop");
	createEntity("fan");

	// Debug Entities
	createEntity("navmesh-debug");
	createEntity("test");
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
int Scene::entityCount()
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

void Scene::init(PhysicsSystem* physics)
{
	camera.initPhysics(physics);
}