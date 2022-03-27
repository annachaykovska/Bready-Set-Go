#include "WaypointUpdater.h"

namespace
{
	const float THRESHOLD = 9.0f;
	const float INTERPOLATOR_THRESHOLD = 1.0f;
	const float LEAD_DISTANCE = 5.0f;

	position INVALID_POS = position(0.f, -99999999.f, 0.f);
}

WaypointUpdater::WaypointUpdater(Entity& entity, NavMesh& navMesh)
	: entity_(entity)
	, navMesh_(navMesh)
	, replanFlag_(false)
{
}

void WaypointUpdater::setWaypoints(std::vector<NavMesh::MeshSegment*> waypoints)
{
	waypoints_ = waypoints;
}

void WaypointUpdater::setTarget(position target)
{
	currentTarget_ = target;
	//waypoints_.insert(waypoints_.begin(), target);
}

void WaypointUpdater::updateWaypoints()
{
	if (waypoints_.size() > 0)
	{
		/*if (length(entity_.getTransform()->position - currentWaypoint()) < THRESHOLD)*/
		if(navMesh_.currentMeshSegment(entity_.getTransform()->position) == currentWaypoint()->id_)
		{
			waypoints_.pop_back();
		}
		//for (int i = 1; i < waypoints_.size(); i++)
		//{
		//	if (futureWaypoint(i) != INVALID_POS)
		//	{
		//		if (length(entity_.getTransform()->position - futureWaypoint(i)) < length(entity_.getTransform()->position - currentWaypoint()))
		//		{
		//			waypoints_.pop_back();
		//		}
		//	}
		//}
	}
}

bool WaypointUpdater::pathComplete()
{
	if (waypoints_.size() == 0)
	{
		replanFlag_ = true;
	}
	else
	{
		replanFlag_ = false;
	}
	return replanFlag_;
}

int WaypointUpdater::numWaypoints()
{
	return waypoints_.size();
}

NavMesh::MeshSegment* WaypointUpdater::currentWaypoint()
{
	if (waypoints_.size() > 0)
	{
		return waypoints_.back();
	}
	else
	{
		return nullptr;
	}
}

NavMesh::MeshSegment* WaypointUpdater::futureWaypoint(int iter)
{
	if (waypoints_.size() - (iter + 1) > 0)
	{
		return waypoints_.at(waypoints_.size() - (iter + 1));
	}
	return nullptr;
}

position WaypointUpdater::interpolator()
{
	interpolator_ = entity_.getTransform()->position;
	//NavMesh::MeshSegment* target = currentWaypoint();
	//int targetIndex = waypoints_.size() - 1;

	//float distance = length(interpolator_ - entity_.getTransform()->position);
	//glm::vec3 direction = normalize(target - interpolator_);

	//while (distance < LEAD_DISTANCE)
	//{
	//	interpolator_ += direction;
	//	distance += 1;
	//	if (length(target - interpolator_) < INTERPOLATOR_THRESHOLD)
	//	{
	//		targetIndex--;
	//		if (targetIndex > -1)
	//		{
	//			target = waypoints_.at(targetIndex);
	//		}
	//	}
	//	direction = normalize(target - interpolator_);
	//}
	return interpolator_;
}
