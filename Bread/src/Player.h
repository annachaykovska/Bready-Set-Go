#pragma once

#include "Recipe.h"
#include "Ingredient.h"
#include "../Scene/Entity.h"
#include "Inventory.h"


class Player {

public:

	int playerID; //Player 1, 2... etc
	bool isAI;
	Entity player;
	Inventory inv;



private:

	Recipe recipes[4]; //Set of recipes for the player to complete
	float progress;

};