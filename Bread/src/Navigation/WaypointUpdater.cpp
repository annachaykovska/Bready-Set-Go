#include "WaypointUpdater.h"

namespace
{
	const float THRESHOLD = 6.0f;
}

WaypointUpdater::WaypointUpdater(Entity& entity, std::vector<position> waypoints)
	: entity_(entity)
	, waypoints_(waypoints)
{

}

void WaypointUpdater::updateWaypoints()
{
	if (waypoints_.size() > 0)
	{
		if (length(entity_.getTransform()->position - currentWaypoint()) < THRESHOLD)
		{
			std::cout << "Reached waypoint" << std::endl;
			waypoints_.pop_back();
		}
	}
}

position WaypointUpdater::currentWaypoint()
{
	if (waypoints_.size() == 0)
	{
		return glm::vec3(0.0f, 0.0f, 0.0f);
	}
	return waypoints_.back();
}
