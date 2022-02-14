#include "WaypointUpdater.h"

namespace
{
	const float THRESHOLD = 9.0f;
	const float INTERPOLATOR_THRESHOLD = 1.0f;
	const float LEAD_DISTANCE = 20.0f;
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

position WaypointUpdater::interpolator()
{
	interpolator_ = entity_.getTransform()->position;
	position target = currentWaypoint();
	int targetIndex = waypoints_.size() - 1;

	float distance = length(interpolator_ - entity_.getTransform()->position);
	glm::vec3 direction = normalize(target - interpolator_);

	while (distance < LEAD_DISTANCE)
	{
		interpolator_ += direction;
		distance += 1;
		if (length(target - interpolator_) < INTERPOLATOR_THRESHOLD)
		{
			targetIndex--;
			if (targetIndex > -1)
			{
				target = waypoints_.at(targetIndex);
			}
		}
		direction = normalize(target - interpolator_);
	}
	return interpolator_;
}
