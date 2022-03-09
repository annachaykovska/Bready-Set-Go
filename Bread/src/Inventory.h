#pragma once

#include <string>
#include <vector>

#include "Scene/Component.h"

class Inventory : public Component
{
public:
	//PIZZA
	unsigned int cheese; // id == 0;
	unsigned int tomato; // id == 1;
	unsigned int sausage;// id == 2;
	unsigned int dough;// id == 3;

	//SALAD
	unsigned int carrot;// id == 4;
	unsigned int lettuce;// id == 5;
	unsigned int parsnip;// id == 6;
	//tomato

	//RICE BOWL
	unsigned int rice;// id == 7;
	unsigned int egg;// id == 8;
	unsigned int chicken;// id == 9;

	//SOUP
	unsigned int peas;// id == 10;
	unsigned int soupbase;// id == 11;
	unsigned int pumpkin;// id == 12;
	//carrot

	Inventory()
	{
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
		this->soupbase = 0;
		this->pumpkin = 0;
	}

	bool checkPizza()
	{
		if (this->cheese == 1 && this->dough == 1 && this->sausage == 1 && this->tomato == 1)
			return true;
		else
			return false;
	}

	void setIngredientFromId(int ingredient) {
		switch (ingredient)
		{
		case (0):
			this->cheese = 1;
			break;
		case (1):
			this->dough = 1;
			break;
		case (2):
			this->sausage = 1;
			break;
		case (3):
			this->tomato = 1;
			break;
		case (4):
			this->carrot = 1;
			break;
		case (5):
			this->lettuce = 1;
			break;
		case (6):
			this->parsnip = 1;
			break;
		case (7):
			this->rice = 1;
			break;
		case (8):
			this->egg = 1;
			break;
		case (9):
			this->chicken = 1;
			break;
		case (10):
			this->peas = 1;
			break;
		case (11):
			this->soupbase = 1;
			break;
		case (12):
			this->pumpkin = 1;
			break;
		default:
			break;
		}
	}

	int removeRandomPizzaIngredient(int opponentCheese, int opponentDough, int opponentSausage, int opponentTomato) {
		// TODO add randomizer
		if (this->cheese == 0 && this->dough == 0 && this->sausage == 0 && this->tomato == 0)
			return -1;
		else {
			if (opponentCheese == 0 && this->cheese == 1) {
				this->cheese = 0;
				return 0;
			}
			else if (opponentDough == 0 && this->dough == 1) {
				this->dough = 0;
				return 1;
			}
			else if (opponentSausage == 0 && this->sausage == 1) {
				this->sausage = 0;
				return 2;
			}
			else if (opponentTomato == 0 && this->tomato == 1) {
				this->tomato = 0;
				return 3;
			}
		}
		return -1;
	}
};
