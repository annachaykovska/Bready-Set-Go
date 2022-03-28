#pragma once

#include <string>
#include <vector>

#include "Scene/Component.h"

class Inventory : public Component
{
public:
	unsigned int cheese; // id == 0;
	unsigned int tomato; // id == 1;
	unsigned int sausage;// id == 2;
	unsigned int dough;// id == 3;
	unsigned int carrot;// id == 4;
	unsigned int lettuce;// id == 5;
	unsigned int parsnip;// id == 6;
	unsigned int rice;// id == 7;
	unsigned int egg;// id == 8;
	unsigned int chicken;// id == 9;
	unsigned int peas;// id == 10;

	Inventory()
	{
		clearAllIngredients();
	}

	void clearAllIngredients() {
		this->cheese = 0;
		this->dough = 0;
		this->sausage = 0;
		this->tomato = 0;
		this->carrot = 0;
		this->lettuce = 0;
		this->parsnip = 0;
		this->rice = 0;
		this->egg = 0;
		this->chicken = 0;
		this->peas = 0;
		//this->soupbase = 0;
		//this->pumpkin = 0;
	}
};
