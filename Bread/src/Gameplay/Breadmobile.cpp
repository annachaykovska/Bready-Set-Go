#include "Breadmobile.h"
#include "../Scene/Entity.h"
#include "../Inventory.h"
#include "Recipe.h"
#include <string>

using namespace std;

Breadmobile::Breadmobile(Entity e, Inventory i, int id) {

	this->ent = e;
	this->inv = i;
	this->playerID = id;

	Recipe r1, r2, r3;

	switch (id) {

	//Recipe sets are predetermined as of now, will potentially randomize later
	case 1: //PLAYER 1 RECIPE SET
		r1 = Recipe(0); //First recipe is pizza
		r2 = Recipe(1); //Second is salad
		r3 = Recipe(2); //Third is rice bowl

		this->recipes[0] = r1;
		this->recipes[1] = r2;
		this->recipes[2] = r3;
		this->currentRecipeID = 0;


	case 2://PLAYER 2 RECIPE SET
		r1 = Recipe(3);
		r2 = Recipe(2);
		r3 = Recipe(1);
		this->recipes[0] = r1;
		this->recipes[1] = r2;
		this->recipes[2] = r3;
		this->currentRecipeID = 3;

	case 3: //PLAYER 3 RECIPE SET
		r1 = Recipe(2); //First recipe is pizza
		r2 = Recipe(0); //Second is salad
		r3 = Recipe(3); //Third is rice bowl

		this->recipes[0] = r1;
		this->recipes[1] = r2;
		this->recipes[2] = r3;
		this->currentRecipeID = 2;

	case 4: //PLAYER 4 RECIPE SET
		r1 = Recipe(1); //First recipe is pizza
		r2 = Recipe(3); //Second is salad
		r3 = Recipe(2); //Third is rice bowl

		this->recipes[0] = r1;
		this->recipes[1] = r2;
		this->recipes[2] = r3;
		this->currentRecipeID = 1;
	}
}

void Breadmobile::checkProgress() {

	//First update the progress of the recipe
	this->recipes[this->currentRecipeID].updateRecipeProgress(this->inv);

	//TL;DR.. Check the progress of the recipe this player is currently working on
	if (this->recipes[this->currentRecipeID].complete == true) {

		printf("PLAYER %d : Recipe %d completed!\n", this->playerID, this->currentRecipeID);

		//Set finished recipe to inactive, set next recipe to active
		//this->recipes[this->progress].active = false;
		//this->recipes[this->progress+1].active = true;

		this->progress = this->progress + 1; //Increment progress- note that progress is also equal to the array index of the next recipe
		this->currentRecipeID = this->recipes[this->progress].id; //Set current recipe id to the next on the list
	
	}



}