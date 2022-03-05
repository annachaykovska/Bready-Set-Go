#pragma once

#include "Recipe.h"
#include "Ingredient.h"
#include "Ingredient.cpp"

Recipe::Recipe(int id) {

	this->id = id;
	active = true;
	progress = 0;

	//Initialize recipe ingredients depending on recipe type id....
	//0==pizza,1==cake,2==??
	switch (id) {
	case 0:
		Ingredient * cheese = new Ingredient(0);
		Ingredient* tomato = new Ingredient(1);
		Ingredient* dough = new Ingredient(2);
		Ingredient* sausage = new Ingredient(3);

		//this->ingredients[0] = cheese;
		//this->ingredients[1] = tomato;
		//this->ingredients[2] = dough;
		//this->ingredients[3] = sausage;

	case 1:
		//Ingredient* strawberry(4);
		//Ingredient* icing(5);
		//Ingredient* flour(6);
		//Ingredient* egg(7);

		//this->ingredients[0] = strawberry;
		//this->ingredients[1] = icing;
		//this->ingredients[2] = flour;
		//this->ingredients[3] = egg;

	case 2:
		Ingredient cheese(0);
		Ingredient tomato(1);
		Ingredient dough(2);
		Ingredient sausage(3);

		//this->ingredients[0] = cheese;
		//this->ingredients[1] = tomato;
		//this->ingredients[2] = dough;
		//this->ingredients[3] = sausage;

	case 3:
		Ingredient cheese(0);
		Ingredient tomato(1);
		Ingredient dough(2);
		Ingredient sausage(3);

		//this->ingredients[0] = cheese;
		//this->ingredients[1] = tomato;
		//this->ingredients[2] = dough;
		//this->ingredients[3] = sausage;

	}
}

void Recipe::updateRecipeProgress() {

	if (this->progress == 1) {
		active = false;
	}
}

