#pragma once
#include "../Transform.h"

class IngredientTracker
{
public:
	IngredientTracker(Transform& cheese, Transform& tomato, Transform& dough, Transform& sausage);

	void updateCheeseTransformSource(Transform& source);
	void updateTomatoTransformSource(Transform& source);
	void updateDoughTransformSource(Transform& source);
	void updateSausageTransformSource(Transform& source);

	Transform getCheeseLocation();
	Transform getTomatoLocation();
	Transform getDoughLocation();
	Transform getSausageLocation();

private:
	Transform& cheeseLocation;
	Transform& tomatoLocation;
	Transform& doughLocation;
	Transform& sausageLocation;
};