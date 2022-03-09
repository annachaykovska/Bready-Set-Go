#pragma once
#include "../Inventory.h"
#include "IngredientTracker.h"
#include "NavigationSystem.h"

enum target {CHEESE, TOMATO, DOUGH, SAUSAGE, NONE};

class AIBrain
{
public:
	AIBrain(Inventory& inventory, IngredientTracker& tracker, NavigationSystem& navigation);

	void pause();
	void update();

private:
	target findClosestTarget();
	int getInventoryCount();

	target currentTarget;

	position trackedTargetDelta;

	Inventory& inventory;
	IngredientTracker& tracker;
	NavigationSystem& navigation;

	int oldInventoryCount;

	int panicCounter_;
	float distance_;
};
