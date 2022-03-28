#pragma once
#include "../Inventory.h"
#include "IngredientTracker.h"
#include "NavigationSystem.h"
#include "../Gameplay/Recipe.h"

class AIBrain
{
public:
	AIBrain(Inventory& inventory, IngredientTracker& tracker, NavigationSystem& navigation);

	void pause();
	void update();
	void setRecipe(Recipe* recipe);

private:
	Ingredient findClosestTarget();
	int getInventoryCount();

	Ingredient currentTarget;

	position trackedTargetDelta;

	Inventory& inventory;
	IngredientTracker& tracker;
	NavigationSystem& navigation;

	Recipe* currentRecipe;

	int oldInventoryCount;

	int panicCounter_;
	float distance_;
};
