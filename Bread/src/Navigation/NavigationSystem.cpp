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
	waypointUpdater_.setWaypoints(pathFinder_.findPath(glm::vec3(-90.f, -1.f, 95.f), target));
	//std::cout << "Planned Path Length" << std::endl;
	//std::cout << waypointUpdater_.numWaypoints() << std::endl;
}

void NavigationSystem::update()
{
	if (waypointUpdater_.pathComplete())
	{
		//std::cout << "Finished Path" << std::endl;
		waypointUpdater_.setWaypoints(pathFinder_.findPath(vehicle_.getTransform()->position, glm::vec3(-90.f, 0.f, 95.f)));
	}

	steering_.updateSteering(waypointUpdater_.interpolator());
	waypointUpdater_.updateWaypoints();
}