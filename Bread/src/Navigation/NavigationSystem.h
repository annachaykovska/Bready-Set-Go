#pragma once
#include "LocationTracker.h"
#include "WaypointUpdater.h"
#include "NavMesh.h"
#include "PathFinder.h"
#include "Steering.h"
#include "../Scene/Component.h"
#include "../Scene/Entity.h"
#include "../Physics/PhysicsSystem.h"

enum NavMode { nav, search };

class NavigationSystem : public Component
{
public:
	NavigationSystem(Entity& vehicle, PhysicsSystem& physics, NavMesh& navmesh, int id);

	void planPath(position target);
	void pause();
	void update();

	void setMode(NavMode mode);
	float distanceFromVehicle(position target);
	bool hasPath();
	void coolDownMagnet();

	bool lostPath();
	bool queryReset();

	PhysicsSystem& physics_;

private:
	Entity& vehicle_;
	NavMesh& navmesh_;

	WaypointUpdater waypointUpdater_;
	PathFinder pathFinder_;

	int id_;
	bool resetFlag_;
	bool lostPath_;
	Steering steering_;

	position currentTarget_;
	NavMode currentMode_;

	float magnetCooldown_;
	float searchWatchdog_;
};