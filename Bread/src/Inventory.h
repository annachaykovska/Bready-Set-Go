#pragma once

#include <string>
#include <vector>

#include "Scene/Component.h"

class Inventory : public Component
{
public:
	unsigned int cheese;
	unsigned int tomato;
	unsigned int sausage;
	unsigned int dough;

	Inventory()
	{
		this->cheese = 0;
		this->dough = 0;
		this->sausage = 0;
		this->tomato = 0;
	}

	bool checkPizza()
	{
		if (this->cheese == 1 && this->dough == 1 && this->sausage == 1 && this->tomato == 1)
			return true;
		else
			return false;
	}
};