#include "Scene.h"
#include "Entity.h"

Scene::Scene()
{
	this->entities.reserve(20);
	this->p1Camera = new Camera();
	this->p2Camera = new Camera();
	this->p3Camera = new Camera();
	this->p4Camera = new Camera();
}

Scene::~Scene()
{
	delete p1Camera;
	delete p2Camera;
	delete p3Camera;
	delete p4Camera;
}

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

	// Adding players to a list to keep track in one place
	players.push_back(getEntity("player1"));
	players.push_back(getEntity("player2"));
	players.push_back(getEntity("player3"));
	players.push_back(getEntity("player4"));

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

	// Adding ingredients to a list to keep track in one place
	ingredients.push_back(getEntity("cheese"));
	ingredients.push_back(getEntity("sausage"));
	ingredients.push_back(getEntity("tomato"));
	ingredients.push_back(getEntity("dough"));
	ingredients.push_back(getEntity("carrot"));
	ingredients.push_back(getEntity("lettuce"));
	ingredients.push_back(getEntity("parsnip"));
	ingredients.push_back(getEntity("rice"));
	ingredients.push_back(getEntity("egg"));
	ingredients.push_back(getEntity("chicken"));
	ingredients.push_back(getEntity("peas"));

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

std::vector<Entity*> Scene::getIngredients() {
	return ingredients;
}
std::vector<Entity*> Scene::getPlayers() {
	return players;
}