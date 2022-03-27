#pragma once
#include "LocationTracker.h"
#include "WaypointUpdater.h"
#include "NavMesh.h"
#include "PathFinder.h"
#include "Steering.h"
#include "../Scene/Component.h"
#include "../Scene/Entity.h"
#include "../Physics/PhysicsSystem.h"

class NavigationSystem : public Component
{
public:
	NavigationSystem(Entity& vehicle, PhysicsSystem& physics, NavMesh& navmesh, int id);

	void planPath(position target);
	void pause();
	void update();

	float distanceFromVehicle(position target);
	bool hasPath();

	bool queryReset();

	PhysicsSystem& physics_;

private:
	Entity& vehicle_;
	NavMesh& navmesh_;

	//LocationTracker locationTracker_;
	WaypointUpdater waypointUpdater_;
	PathFinder pathFinder_;

	int id_;
	bool resetFlag_;
	Steering steering_;

	position currentTarget_;
};