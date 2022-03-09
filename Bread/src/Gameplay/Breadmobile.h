#pragma once
#include "../Inventory.h"
#include "Recipe.h"
#include "../Scene/Entity.h"
#include <string>

using namespace std;

class Breadmobile {
	
public:

	int playerID;		 //Player 1, player 2,... etc
	bool isAI;	         //Is this an AI or player
	int currentRecipeID; //Which recipe do i need to check?

	Entity ent;
	Inventory inv;

	Breadmobile(Entity,Inventory,int);
	void checkProgress();
	
	

private:

	Recipe recipes[3];
	int progress; //Starts at 0, once it gets to 3 the player in question wins!!!

};