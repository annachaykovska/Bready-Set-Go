#pragma once
#include "../Transform.h"
#include "../Scene/Scene.h"
#include "../Inventory.h"
#include "../Scene/Entity.h"

class IngredientTracker
{
public:
	IngredientTracker();

	void update();

	void updateCheeseTransformSource(Transform* source);
	void updateDoughTransformSource(Transform* source);
	void updateSausageTransformSource(Transform* source);
	void updateTomatoTransformSource(Transform* source);
	void updateCarrotTransformSource(Transform* source);
	void updateLettuceTransformSource(Transform* source);
	void updateParsnipTransformSource(Transform* source);
	void updateRiceTransformSource(Transform* source);
	void updateEggTransformSource(Transform* source);
	void updateChickenTransformSource(Transform* source);
	void updatePeasTransformSource(Transform* source);

	Transform getCheeseLocation();
	Transform getDoughLocation();
	Transform getSausageLocation();
	Transform getTomatoLocation();
	Transform getCarrotLocation();
	Transform getLettuceLocation();
	Transform getParsnipLocation();
	Transform getRiceLocation();
	Transform getEggLocation();
	Transform getChickenLocation();
	Transform getPeasLocation();

private:
	Transform* cheeseLocation;
	Transform* doughLocation;
	Transform* sausageLocation;
	Transform* tomatoLocation;
	Transform* carrotLocation;
	Transform* lettuceLocation;
	Transform* parsnipLocation;
	Transform* riceLocation;
	Transform* eggLocation;
	Transform* chickenLocation;
	Transform* peasLocation;
};