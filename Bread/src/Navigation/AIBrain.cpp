#include "AIBrain.h"

AIBrain::AIBrain(Inventory& inventory, IngredientTracker& tracker, NavigationSystem& navigation)
	: inventory(inventory)
	, tracker(tracker)
	, navigation(navigation)
	, oldInventoryCount(0)
	, panicCounter_(0)
	, distance_(0)
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
	if (!navigation.hasPath() || oldInventoryCount != getInventoryCount() || navigation.queryReset())
	{
		panicCounter_ = 0;
		oldInventoryCount = getInventoryCount();
		target newTarget = findClosestTarget();
		position targetPosition;
		switch (newTarget)
		{
		case CHEESE:
			targetPosition = tracker.getCheeseLocation().position;
			break;
		case TOMATO:
			targetPosition = tracker.getTomatoLocation().position;
			break;
		case DOUGH:
			targetPosition = tracker.getDoughLocation().position;
			break;
		case SAUSAGE:
			targetPosition = tracker.getSausageLocation().position;
			break;
		default:
			targetPosition = glm::vec3(-80.f, 0.f, 60.f);
			break;
		}
		navigation.planPath(targetPosition);
	}
	navigation.update();
}

target AIBrain::findClosestTarget()
{
	distance_ = 999999;
	target bestTarget = NONE;
	if (inventory.cheese == 0 && navigation.distanceFromVehicle(tracker.getCheeseLocation().position) < distance_)
	{
		bestTarget = CHEESE;
		distance_ = navigation.distanceFromVehicle(tracker.getCheeseLocation().position);
	}
	if (inventory.tomato == 0 && navigation.distanceFromVehicle(tracker.getTomatoLocation().position) < distance_)
	{
		bestTarget = TOMATO;
		distance_ = navigation.distanceFromVehicle(tracker.getTomatoLocation().position);
	}
	if (inventory.dough == 0 && navigation.distanceFromVehicle(tracker.getDoughLocation().position) < distance_)
	{
		bestTarget = DOUGH;
		distance_ = navigation.distanceFromVehicle(tracker.getDoughLocation().position);
	}
	if (inventory.sausage == 0 && navigation.distanceFromVehicle(tracker.getSausageLocation().position) < distance_)
	{
		bestTarget = SAUSAGE;
		distance_ = navigation.distanceFromVehicle(tracker.getSausageLocation().position);
	}

	return bestTarget;
}

int AIBrain::getInventoryCount()
{
	return inventory.cheese + inventory.tomato + inventory.dough + inventory.sausage;
}