#include "NavigationSystem.h"

NavigationSystem::NavigationSystem(Entity& vehicle, PhysicsSystem& physics, NavMesh& navmesh)
	: vehicle_(vehicle)
	, physics_(physics)
	, navmesh_(navmesh)
	, pathFinder_(navmesh_)
	, steering_(vehicle_, physics_)
	, waypointUpdater_(vehicle_)
{
}

void NavigationSystem::planPath(position target)
{
	if (vehicle_.getTransform()->position != glm::vec3(0.f, 0.f, 0.f))
	{
		waypointUpdater_.setWaypoints(pathFinder_.findPath(vehicle_.getTransform()->position, target));
		waypointUpdater_.setTarget(target);
	}
	//std::cout << "Planned Path Length" << std::endl;
	//std::cout << waypointUpdater_.numWaypoints() << std::endl;
}

void NavigationSystem::update()
{
	if (hasPath())
	{
		steering_.updateSteering(waypointUpdater_.interpolator());
		waypointUpdater_.updateWaypoints();
	}
	else
	{
		std::cout << "PATH COMPLETE" << std::endl;
		steering_.park();
	}
	if (vehicle_.getTransform()->position.y < -30)
	{
		// TODO:NAV
		physics_.respawnPlayer(1);
	}
}

float NavigationSystem::distanceFromVehicle(position target)
{
	return length(vehicle_.getTransform()->position - target);
}

bool NavigationSystem::hasPath()
{
	if (waypointUpdater_.pathComplete())
	{
		return false;
	}
	return true;
}