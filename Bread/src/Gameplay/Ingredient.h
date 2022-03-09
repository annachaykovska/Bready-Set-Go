#pragma once
#include "../Scene/Entity.h"


class Ingredient {

public:
	int id;
	int quantity;
	Entity entity;

	Ingredient(int, int, Entity);
	Ingredient();

};