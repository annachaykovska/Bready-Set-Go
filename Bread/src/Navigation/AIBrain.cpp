#include "AIBrain.h"

AIBrain::AIBrain(Inventory& inventory, IngredientTracker& tracker, NavigationSystem& navigation)
	: inventory(inventory)
	, tracker(tracker)
	, navigation(navigation)
	, oldInventoryCount(0)
{

}

void AIBrain::update()
{
	if (!navigation.hasPath() || oldInventoryCount != getInventoryCount())
	{
		oldInventoryCount = getInventoryCount();
		target newTarget = findClosestTarget();
		position targetPosition;
		switch (newTarget)
		{
		case CHEESE:
			std::cout << "GETTING CHEESE" << std::endl;
			targetPosition = tracker.getCheeseLocation().position;
			break;
		case TOMATO:
			std::cout << "GETTING TOMATO" << std::endl;
			targetPosition = tracker.getTomatoLocation().position;
			break;
		case DOUGH:
			std::cout << "GETTING DOUGH" << std::endl;
			targetPosition = tracker.getDoughLocation().position;
			break;
		case SAUSAGE:
			std::cout << "GETTING SAUSAGE" << std::endl;
			targetPosition = tracker.getSausageLocation().position;
			break;
		default:
			std::cout << "ALL DONE" << std::endl;
			targetPosition = glm::vec3(-80.f, 0.f, 60.f);
			break;
		}
		navigation.planPath(targetPosition);
	}
	navigation.update();
}

target AIBrain::findClosestTarget()
{
	float shortestDistance = 999999;
	target bestTarget = NONE;
	if (inventory.cheese == 0 && navigation.distanceFromVehicle(tracker.getCheeseLocation().position) < shortestDistance)
	{
		bestTarget = CHEESE;
		shortestDistance = navigation.distanceFromVehicle(tracker.getCheeseLocation().position);
	}
	if (inventory.tomato == 0 && navigation.distanceFromVehicle(tracker.getTomatoLocation().position) < shortestDistance)
	{
		bestTarget = TOMATO;
		shortestDistance = navigation.distanceFromVehicle(tracker.getTomatoLocation().position);
	}
	if (inventory.dough == 0 && navigation.distanceFromVehicle(tracker.getDoughLocation().position) < shortestDistance)
	{
		bestTarget = DOUGH;
		shortestDistance = navigation.distanceFromVehicle(tracker.getDoughLocation().position);
	}
	if (inventory.sausage == 0 && navigation.distanceFromVehicle(tracker.getSausageLocation().position) < shortestDistance)
	{
		bestTarget = SAUSAGE;
		shortestDistance = navigation.distanceFromVehicle(tracker.getSausageLocation().position);
	}

	return bestTarget;
}

int AIBrain::getInventoryCount()
{
	return inventory.cheese + inventory.tomato + inventory.dough + inventory.sausage;
}