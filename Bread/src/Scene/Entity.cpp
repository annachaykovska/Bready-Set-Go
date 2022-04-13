#include <string>
#include <vector>

#include "Entity.h"
#include "Scene.h"
#include "../Gameplay/Recipe.h"
#include "../SystemManager.h"

extern Scene g_scene;
extern SystemManager g_systems;

using namespace std;

Entity::Entity(std::string name)
{
	this->name = name;
	this->originalSpawn;
	this->bannerSoundPlayed = false;
	this->removeFlag = false;
	this->orientation = glm::vec3(1.0f);
	this->verifyPlayerCollision = false;
	this->otherPlayerInCollision = "";
	this->collisionCooldownStart = -1;

	//Magnet variables
	int magnetStatus = 0;
	this->magnetDistanceSquared = 400.f;
	this->magnetCooldown = 4.0f;
	this->lastMagnetUse = 0.f;
	this->gracePeriod = 6.0f;
	this->lastGracePeriodStart = 0.f;
	this->tethered = false;
	this->magnetStartTime = 0.f;
	this->magnetTimeToSteal = 0.5f;
	this->unflipTimer = 0.f;

	this->ingredientCollectDistanceSquared = 400.f;

}

bool Entity::attachComponent(Component* newComponent, std::string name)
{
	if ((components.emplace(name, newComponent)).second)
	{
		newComponent->owner = this;
		return true;
	}
	else
		return false;
}

Component* Entity::getComponent(std::string key)
{
	auto result = components.find(key);

	if (result != components.end())
		return result->second;
	else
		return nullptr;
}

Transform* Entity::getTransform()
{
	return (Transform*)(this->getComponent("transform"));
}

Model* Entity::getModel()
{
	Model* result = (Model*)(this->getComponent("model"));
	
	if (!result)
		std::cout << "Model component not found!\n";
	
	return result;
}

AudioSource* Entity::getAudioSource()
{
	return (AudioSource*)(this->getComponent("audio"));
}

Inventory* Entity::getInventory()
{
	return (Inventory*)this->getComponent("inventory");
}

std::vector<std::string> Entity::getOtherPlayers() {
	std::vector<std::string> result = {};
	if (this->name == "player1") {
		result = { "player2", "player3", "player4" };
	}
	else if (this->name == "player2") {
		result = { "player1", "player3", "player4" };
	}
	else if (this->name == "player3") {
		result = { "player1", "player2", "player4" };
	}
	else if (this->name == "player4") {
		result = { "player1", "player2", "player3" };
	}

	return result;
}

void Entity::checkIngredientCollision(Entity* otherEntity) {
	Inventory* playerInventory = (Inventory*)this->getComponent("inventory");
	Recipe* recipe = (Recipe*)(this->getComponent("recipe"));

	if (otherEntity->name == "cheese" && playerInventory->cheese == 0 && recipe->checkRecipeForIngredient(Ingredient::Cheese))
	{
		playerInventory->cheese = 1;
		playerInventory->checkRecipeComplete(this);
		if (playerInventory->allIngredientsCollected && !this->bannerSoundPlayed) {
			bannerSoundPlayed = true;
			this->getAudioSource()->play("banner_popup.wav");
		}
		else {
			this->getAudioSource()->play("pickup.wav");
		}
		//entity0->getModel()->visible = false;
		otherEntity->removeFlag = true;
	}
	if (otherEntity->name == "dough" && playerInventory->dough == 0 && recipe->checkRecipeForIngredient(Ingredient::Dough))
	{
		playerInventory->dough = 1;
		playerInventory->checkRecipeComplete(this);
		if (playerInventory->allIngredientsCollected && !this->bannerSoundPlayed) {
			bannerSoundPlayed = true;
			this->getAudioSource()->play("banner_popup.wav");
		}
		else {
			this->getAudioSource()->play("pickup.wav");
		}
		//entity0->getModel()->visible = false;
		otherEntity->removeFlag = true;
	}
	if (otherEntity->name == "sausage" && playerInventory->sausage == 0 && recipe->checkRecipeForIngredient(Ingredient::Sausage))
	{
		playerInventory->sausage = 1;
		playerInventory->checkRecipeComplete(this);
		if (playerInventory->allIngredientsCollected && !this->bannerSoundPlayed) {
			bannerSoundPlayed = true;
			this->getAudioSource()->play("banner_popup.wav");
		}
		else {
			this->getAudioSource()->play("pickup.wav");
		}
		//entity0->getModel()->visible = false;
		otherEntity->removeFlag = true;
	}
	if (otherEntity->name == "tomato" && playerInventory->tomato == 0 && recipe->checkRecipeForIngredient(Ingredient::Tomato))
	{
		playerInventory->tomato = 1;
		playerInventory->checkRecipeComplete(this);
		if (playerInventory->allIngredientsCollected && !this->bannerSoundPlayed) {
			bannerSoundPlayed = true;
			this->getAudioSource()->play("banner_popup.wav");
		}
		else {
			this->getAudioSource()->play("pickup.wav");
		}
		//entity0->getModel()->visible = false;
		otherEntity->removeFlag = true;
	}
	if (otherEntity->name == "carrot" && playerInventory->carrot == 0 && recipe->checkRecipeForIngredient(Ingredient::Carrot))
	{
		playerInventory->carrot = 1;
		playerInventory->checkRecipeComplete(this);
		if (playerInventory->allIngredientsCollected && !this->bannerSoundPlayed) {
			bannerSoundPlayed = true;
			this->getAudioSource()->play("banner_popup.wav");
		}
		else {
			this->getAudioSource()->play("pickup.wav");
		}
		//entity0->getModel()->visible = false;
		otherEntity->removeFlag = true;
	}
	if (otherEntity->name == "lettuce" && playerInventory->lettuce == 0 && recipe->checkRecipeForIngredient(Ingredient::Lettuce))
	{
		playerInventory->lettuce = 1;
		playerInventory->checkRecipeComplete(this);
		if (playerInventory->allIngredientsCollected && !this->bannerSoundPlayed) {
			bannerSoundPlayed = true;
			this->getAudioSource()->play("banner_popup.wav");
		}
		else {
			this->getAudioSource()->play("pickup.wav");
		}
		//entity0->getModel()->visible = false;
		otherEntity->removeFlag = true;
	}
	if (otherEntity->name == "parsnip" && playerInventory->parsnip == 0 && recipe->checkRecipeForIngredient(Ingredient::Parsnip))
	{
		playerInventory->parsnip = 1;
		playerInventory->checkRecipeComplete(this);
		if (playerInventory->allIngredientsCollected && !this->bannerSoundPlayed) {
			bannerSoundPlayed = true;
			this->getAudioSource()->play("banner_popup.wav");
		}
		else {
			this->getAudioSource()->play("pickup.wav");
		}
		//entity0->getModel()->visible = false;
		otherEntity->removeFlag = true;
	}
	if (otherEntity->name == "rice" && playerInventory->rice == 0 && recipe->checkRecipeForIngredient(Ingredient::Rice))
	{
		playerInventory->rice = 1;
		playerInventory->checkRecipeComplete(this);
		if (playerInventory->allIngredientsCollected && !this->bannerSoundPlayed) {
			bannerSoundPlayed = true;
			this->getAudioSource()->play("banner_popup.wav");
		}
		else {
			this->getAudioSource()->play("pickup.wav");
		}
		//entity0->getModel()->visible = false;
		otherEntity->removeFlag = true;
	}
	if (otherEntity->name == "egg" && playerInventory->egg == 0 && recipe->checkRecipeForIngredient(Ingredient::Egg))
	{
		playerInventory->egg = 1;
		playerInventory->checkRecipeComplete(this);
		if (playerInventory->allIngredientsCollected && !this->bannerSoundPlayed) {
			bannerSoundPlayed = true;
			this->getAudioSource()->play("banner_popup.wav");
		}
		else {
			this->getAudioSource()->play("pickup.wav");
		}
		//entity0->getModel()->visible = false;
		otherEntity->removeFlag = true;
	}
	if (otherEntity->name == "chicken" && playerInventory->chicken == 0 && recipe->checkRecipeForIngredient(Ingredient::Chicken))
	{
		playerInventory->chicken = 1;
		playerInventory->checkRecipeComplete(this);
		if (playerInventory->allIngredientsCollected && !this->bannerSoundPlayed) {
			bannerSoundPlayed = true;
			this->getAudioSource()->play("banner_popup.wav");
		}
		else {
			this->getAudioSource()->play("pickup.wav");
		}
		//entity0->getModel()->visible = false;
		otherEntity->removeFlag = true;
	}
	if (otherEntity->name == "peas" && playerInventory->peas == 0 && recipe->checkRecipeForIngredient(Ingredient::Peas))
	{
		playerInventory->peas = 1;
		playerInventory->checkRecipeComplete(this);
		if (playerInventory->allIngredientsCollected && !this->bannerSoundPlayed) {
			bannerSoundPlayed = true;
			this->getAudioSource()->play("banner_popup.wav");
		}
		else {
			this->getAudioSource()->play("pickup.wav");
		}
		//entity0->getModel()->visible = false;
		otherEntity->removeFlag = true;
	}

}

void Entity::checkPlayerCollision(Entity* otherEntity) {
	Inventory* playerInventory = (Inventory*)this->getComponent("inventory");

	vector<string> otherPlayers = this->getOtherPlayers();
	if (otherEntity->name == otherPlayers[0] || otherEntity->name == otherPlayers[1] || otherEntity->name == otherPlayers[2])
	{
		this->getAudioSource()->play("thump.wav");
		Inventory* opponentInv = (Inventory*)otherEntity->getComponent("inventory");

		this->verifyPlayerCollision = true;
		this->otherPlayerInCollision = otherEntity->name;
		//printf("Collision detected between %s and %s, raycast to find actors when updating physics next time\n", this->name.c_str(), otherEntity->name.c_str());
	}
	return;
}

inline bool Entity::IngredientCanBeCollected(std::string name) {
	Inventory* playerInventory = (Inventory*)this->getComponent("inventory");
	Recipe* recipe = (Recipe*)(this->getComponent("recipe"));
	if (name == "cheese") {
		if(playerInventory->cheese == 0 && recipe->checkRecipeForIngredient(Ingredient::Cheese)) return true;
		return false;
	}
	if (name == "sausage") {
		if (playerInventory->sausage == 0 && recipe->checkRecipeForIngredient(Ingredient::Sausage)) return true;
		return false;
	}
	if (name == "tomato") {
		if (playerInventory->tomato == 0 && recipe->checkRecipeForIngredient(Ingredient::Tomato)) return true;
		return false;
	}
	if (name == "dough") {
		if (playerInventory->dough == 0 && recipe->checkRecipeForIngredient(Ingredient::Dough)) return true;
		return false;
	}
	if (name == "carrot") {
		if (playerInventory->carrot == 0 && recipe->checkRecipeForIngredient(Ingredient::Carrot)) return true;
		return false;
	}
	if (name == "lettuce") {
		if (playerInventory->lettuce == 0 && recipe->checkRecipeForIngredient(Ingredient::Lettuce)) return true;
		return false;
	}
	if (name == "parsnip") {
		if (playerInventory->parsnip == 0 && recipe->checkRecipeForIngredient(Ingredient::Parsnip)) return true;
		return false;
	}
	if (name == "rice") {
		if (playerInventory->rice == 0 && recipe->checkRecipeForIngredient(Ingredient::Rice)) return true;
		return false;
	}
	if (name == "egg") {
		if (playerInventory->egg == 0 && recipe->checkRecipeForIngredient(Ingredient::Egg)) return true;
		return false;
	}
	if (name == "chicken") {
		if (playerInventory->chicken == 0 && recipe->checkRecipeForIngredient(Ingredient::Chicken)) return true;
		return false;
	}
	if (name == "peas") {
		if (playerInventory->peas == 0 && recipe->checkRecipeForIngredient(Ingredient::Peas)) return true;
		return false;
	}
	return false;
}

std::vector<Entity*> Entity::getCollectableIngredients() {
	std::vector<Entity*> ingredients = g_scene.getIngredients();
	for (int i = 0; i < ingredients.size();) {
		if (!IngredientCanBeCollected(ingredients[i]->name)) {
			ingredients.erase(ingredients.begin() + i);
			continue;
		}
		i++;
	}
	return ingredients;
}

