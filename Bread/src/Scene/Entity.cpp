#include <string>
#include <vector>

#include "Entity.h"
#include "Scene.h"

extern Scene g_scene;

using namespace std;


Entity::Entity(std::string name)
{
	this->name = name;
	this->removeFlag = false;
	this->orientation = glm::vec3(1.0f);
	this->verifyPlayerCollision = false;
	this->otherPlayerInCollision = "";
	this->collisionCooldownStart = -1;
}

Entity::Entity(){
	this->name = "default";
	this->removeFlag = false;
	this->orientation = glm::vec3(1.0f);
	this->verifyPlayerCollision = false;
	this->otherPlayerInCollision = "";
	this->collisionCooldownStart = -1;
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

	if (otherEntity->name == "tomato" && playerInventory->tomato == 0)
	{
		playerInventory->tomato = 1;
		this->getAudioSource()->play("pickup.wav");
		//entity0->getModel()->visible = false;
		otherEntity->removeFlag = true;
	}
	if (otherEntity->name == "cheese" && playerInventory->cheese == 0)
	{
		playerInventory->cheese = 1;
		this->getAudioSource()->play("pickup.wav");
		//entity0->getModel()->visible = false;
		otherEntity->removeFlag = true;
	}
	if (otherEntity->name == "dough" && playerInventory->dough == 0)
	{
		playerInventory->dough = 1;
		this->getAudioSource()->play("pickup.wav");
		//entity0->getModel()->visible = false;
		otherEntity->removeFlag = true;
	}
	if (otherEntity->name == "sausage" && playerInventory->sausage == 0)
	{
		playerInventory->sausage = 1;
		this->getAudioSource()->play("pickup.wav");
		//entity0->getModel()->visible = false;
		otherEntity->removeFlag = true;
	}

	// Check for completed recipes
	//if (playerInventory->checkPizza())
	//	this->getAudioSource()->play("ding.wav");

	//return;
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
