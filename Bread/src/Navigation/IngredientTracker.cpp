#include "IngredientTracker.h"

extern Scene g_scene;

IngredientTracker::IngredientTracker()
	: cheeseLocation(g_scene.getEntity("cheese")->getTransform())
	, doughLocation(g_scene.getEntity("dough")->getTransform())
	, sausageLocation(g_scene.getEntity("sausage")->getTransform())
	, tomatoLocation(g_scene.getEntity("tomato")->getTransform())
	, carrotLocation(g_scene.getEntity("carrot")->getTransform())
	, lettuceLocation(g_scene.getEntity("lettuce")->getTransform())
	, parsnipLocation(g_scene.getEntity("parsnip")->getTransform())
	, riceLocation(g_scene.getEntity("rice")->getTransform())
	, eggLocation(g_scene.getEntity("egg")->getTransform())
	, chickenLocation(g_scene.getEntity("chicken")->getTransform())
	, peasLocation(g_scene.getEntity("peas")->getTransform())
{

}

void IngredientTracker::update()
{
	Entity* player1 = g_scene.getEntity("player1");
	Inventory* p1Inv = (Inventory*)player1->getComponent("inventory");

	Entity* player2 = g_scene.getEntity("player2");
	Inventory* p2Inv = (Inventory*)player2->getComponent("inventory");

	Entity* player3 = g_scene.getEntity("player3");
	Inventory* p3Inv = (Inventory*)player3->getComponent("inventory");

	Entity* player4 = g_scene.getEntity("player4");
	Inventory* p4Inv = (Inventory*)player4->getComponent("inventory");

	Entity* players[] = { player1, player2, player3, player4 };
	Inventory* inventories[] = { p1Inv, p2Inv, p3Inv, p4Inv };

	for (int i = 0; i < 4; i++)
	{
		if (inventories[i]->cheese)
		{
			updateCheeseTransformSource(players[i]->getTransform());
		}
		if (inventories[i]->dough)
		{
			updateDoughTransformSource(players[i]->getTransform());
		}
		if (inventories[i]->sausage)
		{
			updateSausageTransformSource(players[i]->getTransform());
		}
		if (inventories[i]->tomato)
		{
			updateTomatoTransformSource(players[i]->getTransform());
		}
		if (inventories[i]->carrot)
		{
			updateTomatoTransformSource(players[i]->getTransform());
		}
		if (inventories[i]->lettuce)
		{
			updateTomatoTransformSource(players[i]->getTransform());
		}
		if (inventories[i]->parsnip)
		{
			updateTomatoTransformSource(players[i]->getTransform());
		}
		if (inventories[i]->rice)
		{
			updateTomatoTransformSource(players[i]->getTransform());
		}
		if (inventories[i]->egg)
		{
			updateTomatoTransformSource(players[i]->getTransform());
		}
		if (inventories[i]->chicken)
		{
			updateTomatoTransformSource(players[i]->getTransform());
		}
		if (inventories[i]->peas)
		{
			updateTomatoTransformSource(players[i]->getTransform());
		}
	}
}

void IngredientTracker::updateCheeseTransformSource(Transform* source)
{
	cheeseLocation = source;
}

void IngredientTracker::updateDoughTransformSource(Transform* source)
{
	doughLocation = source;
}

void IngredientTracker::updateSausageTransformSource(Transform* source)
{
	sausageLocation = source;
}

void IngredientTracker::updateTomatoTransformSource(Transform* source)
{
	tomatoLocation = source;
}

void IngredientTracker::updateCarrotTransformSource(Transform* source)
{
	carrotLocation = source;
}

void IngredientTracker::updateLettuceTransformSource(Transform* source)
{
	lettuceLocation = source;
}

void IngredientTracker::updateParsnipTransformSource(Transform* source)
{
	parsnipLocation = source;
}

void IngredientTracker::updateRiceTransformSource(Transform* source)
{
	riceLocation = source;
}

void IngredientTracker::updateEggTransformSource(Transform* source)
{
	eggLocation = source;
}

void IngredientTracker::updateChickenTransformSource(Transform* source)
{
	chickenLocation = source;
}

void IngredientTracker::updatePeasTransformSource(Transform* source)
{
	peasLocation = source;
}

Transform IngredientTracker::getCheeseLocation()
{
	return *cheeseLocation;
}

Transform IngredientTracker::getDoughLocation()
{
	return *doughLocation;
}

Transform IngredientTracker::getSausageLocation()
{
	return *sausageLocation;
}

Transform IngredientTracker::getTomatoLocation()
{
	return *tomatoLocation;
}

Transform IngredientTracker::getCarrotLocation()
{
	return *carrotLocation;
}

Transform IngredientTracker::getLettuceLocation()
{
	return *lettuceLocation;
}

Transform IngredientTracker::getParsnipLocation()
{
	return *parsnipLocation;
}

Transform IngredientTracker::getRiceLocation()
{
	return *riceLocation;
}

Transform IngredientTracker::getEggLocation()
{
	return *eggLocation;
}

Transform IngredientTracker::getChickenLocation()
{
	return *chickenLocation;
}

Transform IngredientTracker::getPeasLocation()
{
	return *peasLocation;
}