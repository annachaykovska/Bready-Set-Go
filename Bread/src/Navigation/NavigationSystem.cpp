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
	waypointUpdater_.setWaypoints(pathFinder_.findPath(vehicle_.getTransform()->position -glm::vec3(1.f, 2.f, 1.f), target));
}

void NavigationSystem::update()
{
	steering_.updateSteering(waypointUpdater_.currentWaypoint());
	waypointUpdater_.updateWaypoints();
}