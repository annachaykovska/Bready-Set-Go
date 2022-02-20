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
	NavigationSystem(Entity& vehicle, PhysicsSystem& physics, NavMesh& navmesh);

	void planPath(position target);
	void update();

private:
	PhysicsSystem& physics_;
	Entity& vehicle_;
	NavMesh& navmesh_;

	//LocationTracker locationTracker_;
	WaypointUpdater waypointUpdater_;
	PathFinder pathFinder_;
	Steering steering_;
};