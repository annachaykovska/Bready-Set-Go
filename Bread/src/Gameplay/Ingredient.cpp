#include "Ingredient.h"
#include "../Scene/Entity.h"


Ingredient::Ingredient(int id, int quantity, Entity e) {

	this->quantity = quantity;
	this->id = id;
	this->entity = e;
}

Ingredient::Ingredient() {

}