#pragma once
#include <string>
#include "../Inventory.h"
#include "../Scene/Component.h"

enum Ingredient {Cheese, Dough, Sausage, Tomato, Carrot, Lettuce, Parsnip, Rice, Egg, Chicken, Peas};

class Recipe : public Component {

public:

	Recipe(Ingredient i1, Ingredient i2, Ingredient i3, Ingredient i4, std::string recipeName);

	void updateRecipeProgress(Inventory& inv);

	int progress;
	bool complete;
	bool active;

	std::string name;

private:

	Ingredient list[4];

};