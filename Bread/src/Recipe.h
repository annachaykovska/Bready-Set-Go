#pragma once


#include "Ingredient.h"


class Recipe {

public:

	Recipe(int n);
	void updateRecipeProgress();

	float progress;
	bool active;
	int id;

private:

	Ingredient ingredients[4];

};