#include "IngredientTracker.h"

IngredientTracker::IngredientTracker(Transform& cheese, Transform& tomato, Transform& dough, Transform& sausage)
	: cheeseLocation(cheese)
	, tomatoLocation(tomato)
	, doughLocation(dough)
	, sausageLocation(sausage)
{

}

void IngredientTracker::updateCheeseTransformSource(Transform& source)
{
	cheeseLocation = source;
}

void IngredientTracker::updateTomatoTransformSource(Transform& source)
{
	tomatoLocation = source;
}

void IngredientTracker::updateDoughTransformSource(Transform& source)
{
	doughLocation = source;
}

void IngredientTracker::updateSausageTransformSource(Transform& source)
{
	sausageLocation = source;
}

Transform IngredientTracker::getCheeseLocation()
{
	return cheeseLocation;
}

Transform IngredientTracker::getTomatoLocation()
{
	return tomatoLocation;
}

Transform IngredientTracker::getDoughLocation()
{
	return doughLocation;
}

Transform IngredientTracker::getSausageLocation()
{
	return sausageLocation;
}