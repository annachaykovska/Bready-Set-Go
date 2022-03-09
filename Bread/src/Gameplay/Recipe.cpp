
#pragma once
#include "Recipe.h"
#include "Ingredient.h"
#include "../Inventory.h"


Recipe::Recipe() {
}

Recipe::Recipe(int id) {

	this->id = id;
	active = false;
	progress = 0;

	//Potential ingredients
	Ingredient cheese = Ingredient(0, 1);
	Ingredient tomato = Ingredient(1, 1);
	Ingredient sausage = Ingredient(2, 1);
	Ingredient dough = Ingredient(3, 1);

	Ingredient carrot = Ingredient(4, 1);
	Ingredient lettuce = Ingredient(5, 1);
	Ingredient parsnip = Ingredient(6, 1);

	Ingredient rice = Ingredient(7, 1);
	Ingredient egg = Ingredient(8, 1);
	Ingredient chicken = Ingredient(9, 1);

	Ingredient peas = Ingredient(10, 1);
	Ingredient soupbase = Ingredient(11, 1);
	Ingredient pumpkin = Ingredient(12, 1);

	//Initialize recipe ingredients depending on recipe type id....
	//0==pizza,1==salad,2==soup,3==rice bowl
	//Initialize ingredient list depending on given id
	switch (id) {

	case 0: 
		this->ingredients[0] = cheese;
		this->ingredients[1] = tomato;
		this->ingredients[2] = sausage;
		this->ingredients[3] = dough;

	case 1:
		this->ingredients[0] = carrot;
		this->ingredients[1] = lettuce;
		this->ingredients[2] = parsnip;
		this->ingredients[3] = tomato;

	case 2:
		this->ingredients[0] = rice;
		this->ingredients[1] = egg;
		this->ingredients[2] = chicken;
		this->ingredients[3] = carrot;

	case 3:
		this->ingredients[0] = peas;
		this->ingredients[1] = soupbase;
		this->ingredients[2] = pumpkin;
		this->ingredients[3] = carrot;
	}
}

void Recipe::updateRecipeProgress(Inventory in) {

	int id = this->id;

	//Which recipe do we need to compare?
	switch (id) {

	case 0:
		if (in.cheese == 1 && in.tomato == 1 && in.sausage == 1 && in.dough == 1) {
			this->active = false;
			this->complete = true;
		}

	case 1:
		if (in.carrot == 1 && in.tomato == 1 && in.lettuce == 1 && in.parsnip == 1) {
			this->active = false;
			this->complete = true;
		}

	case 2:
		if (in.rice == 1 && in.egg == 1 && in.chicken == 1 && in.carrot == 1) {
			this->active = false;
			this->complete = true;
		}

	case 3:
		if (in.peas == 1 && in.soupbase == 1 && in.pumpkin == 1 && in.carrot == 1) {
			this->active = false;
			this->complete = true;
		}
	}

}