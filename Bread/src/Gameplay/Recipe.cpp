
#pragma once
#include "Recipe.h"

Recipe::Recipe(Ingredient i1, Ingredient i2, Ingredient i3, Ingredient i4, std::string recipeName) 
	: progress(0)
	, complete(false)
	, active(true)
{
	list[0] = i1;
	list[1] = i2;
	list[2] = i3;
	list[3] = i4;

	name = recipeName;
}

void Recipe::updateRecipeProgress(Inventory& inv) 
{
	progress = 0;
	for (int i = 0; i < 4; i++)
	{
		int held;
		switch (list[i])
		{
		case Cheese:
			held = inv.cheese;
			break;
		case Dough:
			held = inv.dough;
			break;
		case Sausage:
			held = inv.sausage;
			break;
		case Tomato:
			held = inv.tomato;
			break;
		case Carrot:
			held = inv.carrot;
			break;
		case Lettuce:
			held = inv.lettuce;
			break;
		case Parsnip:
			held = inv.parsnip;
			break;
		case Rice:
			held = inv.rice;
			break;
		case Egg:
			held = inv.egg;
			break;
		case Chicken:
			held = inv.chicken;
			break;
		case Peas:
			held = inv.peas;
			break;
		}
		progress += held;
	}

	if (progress == 4)
	{
		complete = true;
	}
	else
	{
		complete = false;
	}
}

bool Recipe::checkRecipeForIngredient(Ingredient ingredient) {
	for (int i = 0; i < 4; i++) {
		if (list[i] == ingredient) {
			return true;
		}
	}
	return false;
}
