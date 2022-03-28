#pragma once
#include <string>
#include "../Inventory.h"
#include "../Scene/Component.h"

enum Ingredient {Cheese, Dough, Sausage, Tomato, Carrot, Lettuce, Parsnip, Rice, Egg, Chicken, Peas, NONE};

class Recipe : public Component {

public:

	Recipe(Ingredient i1, Ingredient i2, Ingredient i3, Ingredient i4, std::string recipeName);

	void updateRecipeProgress(Inventory& inv);
	bool checkRecipeForIngredient(Ingredient ingredient);

	int progress;
	bool complete;
	bool active;

	std::string name;

	Ingredient list[4];
};