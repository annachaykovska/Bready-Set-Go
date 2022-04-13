#include "AIBrain.h"

extern Scene g_scene;

AIBrain::AIBrain(Inventory& inventory, IngredientTracker& tracker, NavigationSystem& navigation, position start)
	: inventory(inventory)
	, tracker(tracker)
	, navigation(navigation)
	, start(start)
	, oldInventoryCount(0)
	, returnToBase(false)
	, panicCounter_(0)
	, distance_(0)
	, trackedTargetDelta(position(0.f, 0.f, 0.f))
	, currentTarget(NONE)
{

}

void AIBrain::update()
{
	findClosestTarget();
	if (distance_ < 30)
	{
		panicCounter_++;
		if (panicCounter_ > 3000)
		{
			navigation.pause();
			panicCounter_ = 0;
		}
	}
	bool movingTarget = false;
	position targetPosition = start;
	bool winStatus = returnToBase;

	switch (currentTarget)
	{
	case Ingredient::Cheese:
		targetPosition = tracker.getCheeseLocation().position;
		returnToBase = false;
		//std::cout << "Cheese" << std::endl;
		if (length(tracker.getCheeseLocation().position - trackedTargetDelta) > 5)
		{
			movingTarget = true;
		}
		break;
	case Ingredient::Dough:
		targetPosition = tracker.getDoughLocation().position;
		returnToBase = false;
		//std::cout << "Dough" << std::endl;
		if (length(tracker.getDoughLocation().position - trackedTargetDelta) > 5)
		{
			movingTarget = true;
		}
		break;
	case Ingredient::Sausage:
		targetPosition = tracker.getSausageLocation().position;
		returnToBase = false;
		//std::cout << "Sausage" << std::endl;
		if (length(tracker.getSausageLocation().position - trackedTargetDelta) > 5)
		{
			movingTarget = true;
		}
		break;
	case Ingredient::Tomato:
		targetPosition = tracker.getTomatoLocation().position;
		returnToBase = false;
		//std::cout << "Tomato" << std::endl;
		if (length(tracker.getTomatoLocation().position - trackedTargetDelta) > 5)
		{
			movingTarget = true;
		}
		break;
	case Ingredient::Carrot:
		//std::cout << "Carrot" << std::endl;
		targetPosition = tracker.getCarrotLocation().position;
		returnToBase = false;
		if (length(tracker.getCarrotLocation().position - trackedTargetDelta) > 5)
		{
			movingTarget = true;
		}
		break;
	case Ingredient::Lettuce:
		//std::cout << "Lettuce" << std::endl;
		targetPosition = tracker.getLettuceLocation().position;
		returnToBase = false;
		if (length(tracker.getLettuceLocation().position - trackedTargetDelta) > 5)
		{
			movingTarget = true;
		}
		break;
	case Ingredient::Parsnip:
		//std::cout << "Parsnip" << std::endl;
		targetPosition = tracker.getParsnipLocation().position;
		returnToBase = false;
		if (length(tracker.getParsnipLocation().position - trackedTargetDelta) > 5)
		{
			movingTarget = true;
		}
		break;
	case Ingredient::Rice:
		//std::cout << "Rice" << std::endl;
		targetPosition = tracker.getRiceLocation().position;
		returnToBase = false;
		if (length(tracker.getRiceLocation().position - trackedTargetDelta) > 5)
		{
			movingTarget = true;
		}
		break;
	case Ingredient::Egg:
		//std::cout << "Egg" << std::endl;
		targetPosition = tracker.getEggLocation().position;
		returnToBase = false;
		if (length(tracker.getEggLocation().position - trackedTargetDelta) > 5)
		{
			movingTarget = true;
		}
		break;
	case Ingredient::Chicken:
		//std::cout << "Chicken" << std::endl;
		targetPosition = tracker.getChickenLocation().position;
		returnToBase = false;
		if (length(tracker.getChickenLocation().position - trackedTargetDelta) > 5)
		{
			movingTarget = true;
		}
		break;
	case Ingredient::Peas:
		//std::cout << "Peas" << std::endl;
		targetPosition = tracker.getPeasLocation().position;
		returnToBase = false;
		if (length(tracker.getPeasLocation().position - trackedTargetDelta) > 5)
		{
			movingTarget = true;
		}
		break;
	case Ingredient::NONE:
		returnToBase = true;
		targetPosition = start;
		break;
	}

	if (oldInventoryCount > getInventoryCount())
	{
		navigation.coolDownMagnet();
	}

	if (!navigation.hasPath() || oldInventoryCount != getInventoryCount() || navigation.queryReset() || navigation.lostPath() || movingTarget || returnToBase != winStatus)
	{
		//std::cout << "Path planned with target: " << targetPosition.x << " " << targetPosition.y << std::endl;
		navigation.setMode(NavMode::nav);
		panicCounter_ = 0;
		oldInventoryCount = getInventoryCount();
		currentTarget = findClosestTarget();

		trackedTargetDelta = targetPosition;
		navigation.planPath(targetPosition);
	}
	navigation.update();
}

Ingredient AIBrain::findClosestTarget()
{
	distance_ = 999999;
	Ingredient bestTarget = Ingredient::NONE;

	for (int i = 0; i < 4; i++)
	{
		switch (currentRecipe->list[i])
		{
		case Ingredient::Cheese:
			//if (inventory.cheese == 0 && navigation.distanceFromVehicle(tracker.getCheeseLocation().position) < distance_)
			if(inventory.cheese == 0)
			{
				bestTarget = Ingredient::Cheese;
				float temp = navigation.distanceFromVehicle(tracker.getCheeseLocation().position);
				distance_ = 50;
				if (temp < distance_)
				{
					distance_ = temp;
				}
				//distance_ = navigation.distanceFromVehicle(tracker.getCheeseLocation().position);
			}
			break;
		case Ingredient::Dough:
			//if (inventory.dough == 0 && navigation.distanceFromVehicle(tracker.getDoughLocation().position) < distance_)
			if(inventory.dough == 0)
			{
				bestTarget = Ingredient::Dough;
				float temp = navigation.distanceFromVehicle(tracker.getDoughLocation().position);
				distance_ = 50;
				if (temp < distance_)
				{
					distance_ = temp;
				}
				//distance_ = navigation.distanceFromVehicle(tracker.getDoughLocation().position);
			}
			break;
		case Ingredient::Sausage:
			if (inventory.sausage == 0 && navigation.distanceFromVehicle(tracker.getSausageLocation().position) < distance_)
			{
				bestTarget = Ingredient::Dough;
				distance_ = navigation.distanceFromVehicle(tracker.getSausageLocation().position);
			}
			break;
		case Ingredient::Tomato:
			//if (inventory.tomato == 0 && navigation.distanceFromVehicle(tracker.getTomatoLocation().position) < distance_)
			if (inventory.tomato == 0)
			{
				bestTarget = Ingredient::Tomato;
				float temp = navigation.distanceFromVehicle(tracker.getTomatoLocation().position);
				distance_ = 50;
				if (temp < distance_)
				{
					distance_ = temp;
				}
				//distance_ = navigation.distanceFromVehicle(tracker.getTomatoLocation().position);
			}
			break;
		case Ingredient::Carrot:
			if (inventory.carrot == 0 && navigation.distanceFromVehicle(tracker.getCarrotLocation().position) < distance_)
			{
				bestTarget = Ingredient::Carrot;
				distance_ = navigation.distanceFromVehicle(tracker.getCarrotLocation().position);
			}
			break;
		case Ingredient::Lettuce:
			//if (inventory.lettuce == 0 && navigation.distanceFromVehicle(tracker.getLettuceLocation().position) < distance_)
			if(inventory.lettuce == 0)
			{
				bestTarget = Ingredient::Lettuce;
				float temp = navigation.distanceFromVehicle(tracker.getLettuceLocation().position);
				distance_ = 200;
				if (temp > distance_)
				{
					distance_ = temp;
				}
				//distance_ = navigation.distanceFromVehicle(tracker.getLettuceLocation().position);
			}
			break;
		case Ingredient::Parsnip:
			if (inventory.parsnip == 0 && navigation.distanceFromVehicle(tracker.getParsnipLocation().position) < distance_)
			{
				bestTarget = Ingredient::Parsnip;
				distance_ = navigation.distanceFromVehicle(tracker.getParsnipLocation().position);
			}
			break;
		case Ingredient::Rice:
			if (inventory.rice == 0 && navigation.distanceFromVehicle(tracker.getRiceLocation().position) < distance_)
			{
				bestTarget = Ingredient::Rice;
				distance_ = navigation.distanceFromVehicle(tracker.getRiceLocation().position);
			}
			break;
		case Ingredient::Egg:
			if (inventory.egg == 0 && navigation.distanceFromVehicle(tracker.getEggLocation().position) < distance_)
			{
				bestTarget = Ingredient::Egg;
				distance_ = navigation.distanceFromVehicle(tracker.getEggLocation().position);
			}
			break;
		case Ingredient::Chicken:
			if (inventory.chicken == 0 && navigation.distanceFromVehicle(tracker.getChickenLocation().position) < distance_)
			{
				bestTarget = Ingredient::Chicken;
				distance_ = navigation.distanceFromVehicle(tracker.getChickenLocation().position);
			}
			break;
		case Ingredient::Peas:
			if (inventory.peas == 0 && navigation.distanceFromVehicle(tracker.getPeasLocation().position) < distance_)
			{
				bestTarget = Ingredient::Peas;
				distance_ = navigation.distanceFromVehicle(tracker.getPeasLocation().position);
			}
			break;
		}
	}
	return bestTarget;
}

int AIBrain::getInventoryCount()
{

	return currentRecipe->progress;
}

void AIBrain::setRecipe(Recipe* recipe)
{
	currentRecipe = recipe;
}