#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "NavMesh.h"
#include "../Scene/Entity.h"

typedef glm::vec3 position;

class WaypointUpdater
{
public:
	WaypointUpdater(Entity& entity, NavMesh& navMesh);

	void setWaypoints(std::vector<NavMesh::MeshSegment*> waypoints);
	void setTarget(position target);
	void updateWaypoints();
	bool pathComplete();
	int numWaypoints();
	NavMesh::MeshSegment* currentWaypoint();
	NavMesh::MeshSegment* futureWaypoint(int iter);
	position interpolator();

private:
	Entity& entity_;
	NavMesh& navMesh_;
	std::vector<NavMesh::MeshSegment*> waypoints_;
	
	position interpolator_;
	bool replanFlag_;

	position currentTarget_;
};