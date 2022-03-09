#pragma once
#include "Ingredient.h"
#include "../Inventory.h"


class Recipe {

public:

	Recipe(int n);
	Recipe();

	void updateRecipeProgress(Inventory);

	float progress;
	bool complete;
	bool active;
	int id;

private:

	Ingredient ingredients[4];

};