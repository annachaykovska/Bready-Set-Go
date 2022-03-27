#include "NavigationSystem.h"

NavigationSystem::NavigationSystem(Entity& vehicle, PhysicsSystem& physics, NavMesh& navmesh, int id)
	: vehicle_(vehicle)
	, physics_(physics)
	, navmesh_(navmesh)
	, pathFinder_(navmesh_)
	, id_(id)
	, steering_(vehicle_, physics_, id_)
	, waypointUpdater_(vehicle_, navmesh_)
	, resetFlag_(false)
	, currentMode_(nav)
{
}

void NavigationSystem::planPath(position target)
{
	if (vehicle_.getTransform()->position != glm::vec3(0.f, 0.f, 0.f))
	{
		waypointUpdater_.setWaypoints(pathFinder_.findPath(vehicle_.getTransform()->position, target));
		//waypointUpdater_.setTarget(target);

		currentTarget_ = target;
	}
	//std::cout << "Planned Path Length" << std::endl;
	//std::cout << waypointUpdater_.numWaypoints() << std::endl;
}

void NavigationSystem::pause()
{
	steering_.park();
}

void NavigationSystem::update()
{
	switch (currentMode_)
	{
	case nav:
		//steering_.updateSteering(waypointUpdater_.interpolator());
		steering_.updateSteering(waypointUpdater_.currentWaypoint()->meshStep_->position_);
		//std::cout << waypointUpdater_.interpolator().x << " " << waypointUpdater_.interpolator().y << " " << waypointUpdater_.interpolator().z << std::endl;
		//std::cout << waypointUpdater_.currentWaypoint().x << " " << waypointUpdater_.currentWaypoint().y << " " << waypointUpdater_.currentWaypoint().z << std::endl;
		waypointUpdater_.updateWaypoints();
		if (waypointUpdater_.pathComplete())
		{
			currentMode_ = search;
		}
		break;
	case search:
		steering_.updateSteering(currentTarget_);
		break;
	}

	if (vehicle_.getTransform()->position.y < -70)
	{
		resetFlag_ = true;
		physics_.respawnPlayer(id_);
	}
}

void NavigationSystem::setMode(NavMode mode)
{
	currentMode_ = mode;
}

float NavigationSystem::distanceFromVehicle(position target)
{
	return length(vehicle_.getTransform()->position - target);
}

bool NavigationSystem::hasPath()
{
	if (waypointUpdater_.pathComplete() && currentMode_ == nav)
	{
		return false;
	}
	return true;
}

bool NavigationSystem::queryReset()
{
	bool resetState = resetFlag_;
	if (resetFlag_)
	{
		resetFlag_ = false;
	}
	return resetState;
}