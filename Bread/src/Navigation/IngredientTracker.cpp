#include "IngredientTracker.h"

extern Scene g_scene;

IngredientTracker::IngredientTracker(Transform* cheese, Transform* tomato, Transform* dough, Transform* sausage)
	: cheeseLocation(cheese)
	, tomatoLocation(tomato)
	, doughLocation(dough)
	, sausageLocation(sausage)
{

}

void IngredientTracker::update()
{
	Entity* player2 = g_scene.getEntity("player2");
	Inventory* p2Inv = (Inventory*)player2->getComponent("inventory");

	// TODO: Add other players

	//if (p1Inv.cheese)
	//{
	//	ingredientTracker.updateCheeseTransformSource(*player1->getTransform());
	//}
	//if (p1Inv.tomato)
	//{
	//	ingredientTracker.updateTomatoTransformSource(*player1->getTransform());
	//}
	//if (p1Inv.sausage)
	//{
	//	ingredientTracker.updateSausageTransformSource(*player1->getTransform());
	//}
	//if (p1Inv.dough)
	//{
	//	ingredientTracker.updateDoughTransformSource(*player1->getTransform());
	//}

	if (p2Inv->cheese)
	{
		updateCheeseTransformSource(player2->getTransform());
	}
	if (p2Inv->tomato)
	{
		updateTomatoTransformSource(player2->getTransform());
	}
	if (p2Inv->sausage)
	{
		updateSausageTransformSource(player2->getTransform());
	}
	if (p2Inv->dough)
	{
		updateDoughTransformSource(player2->getTransform());
	}
}

void IngredientTracker::updateCheeseTransformSource(Transform* source)
{
	cheeseLocation = source;
}

void IngredientTracker::updateTomatoTransformSource(Transform* source)
{
	tomatoLocation = source;
}

void IngredientTracker::updateDoughTransformSource(Transform* source)
{
	doughLocation = source;
}

void IngredientTracker::updateSausageTransformSource(Transform* source)
{
	sausageLocation = source;
}

Transform IngredientTracker::getCheeseLocation()
{
	return *cheeseLocation;
}

Transform IngredientTracker::getTomatoLocation()
{
	return *tomatoLocation;
}

Transform IngredientTracker::getDoughLocation()
{
	return *doughLocation;
}

Transform IngredientTracker::getSausageLocation()
{
	return *sausageLocation;
}