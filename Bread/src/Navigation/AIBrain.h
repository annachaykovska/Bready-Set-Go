#pragma once
#include "../Inventory.h"
#include "IngredientTracker.h"
#include "NavigationSystem.h"
#include "../Gameplay/Recipe.h"

class AIBrain
{
public:
	AIBrain(Inventory& inventory, IngredientTracker& tracker, NavigationSystem& navigation, position start);

	void pause();
	void update();
	void setRecipe(Recipe* recipe);

private:
	Ingredient findClosestTarget();
	int getInventoryCount();

	Ingredient currentTarget;

	position trackedTargetDelta;
	position start;

	Inventory& inventory;
	IngredientTracker& tracker;
	NavigationSystem& navigation;

	Recipe* currentRecipe;

	int oldInventoryCount;
	bool returnToBase;

	int panicCounter_;
	float distance_;
};
