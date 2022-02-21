#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "../Scene/Entity.h"

typedef glm::vec3 position;

class WaypointUpdater
{
public:
	WaypointUpdater(Entity& entity);

	void setWaypoints(std::vector<position> waypoints);
	void updateWaypoints();
	bool pathComplete();
	position currentWaypoint();
	position futureWaypoint(int iter);
	position interpolator();

private:
	Entity& entity_;
	std::vector<position> waypoints_;
	
	position interpolator_;
	bool replanFlag_;
};