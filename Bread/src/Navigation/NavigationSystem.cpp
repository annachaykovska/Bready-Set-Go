#include "NavigationSystem.h"

NavigationSystem::NavigationSystem(Entity& vehicle, PhysicsSystem& physics, NavMesh& navmesh, int id)
	: vehicle_(vehicle)
	, physics_(physics)
	, navmesh_(navmesh)
	, pathFinder_(navmesh_)
	, id_(id)
	, steering_(vehicle_, physics_, id_)
	, behaviorHandler_(vehicle_.getTransform())
	, waypointUpdater_(vehicle_, navmesh_)
	, resetFlag_(false)
	, currentMode_(nav)
	, magnetCooldown_(0)
	, searchWatchdog_(0)
{
}

void NavigationSystem::planPath(position target)
{
	if (vehicle_.getTransform()->position != glm::vec3(0.f, 0.f, 0.f))
	{
		waypointUpdater_.setWaypoints(pathFinder_.findPath(vehicle_.getTransform()->position, target));
		currentTarget_ = target;
	}
}

void NavigationSystem::update()
{
	float currentTime = glfwGetTime();
	if (currentTime - magnetCooldown_ > 3)
	{
		std::cout << "Magnet Ready" << std::endl;
	}
	else
	{
		std::cout << "Waiting" << std::endl;
	}
	if (length(currentTarget_ - vehicle_.getTransform()->position) < 20)
	{
		if (currentTime - magnetCooldown_ > 3)
		{
			physics_.magnet(id_);
		}
	}
	switch (currentMode_)
	{
	case nav:
		lostPath_ = waypointUpdater_.offPath();
		if (waypointUpdater_.currentWaypoint() != nullptr)
		{
			steering_.updateSteering(waypointUpdater_.currentWaypoint()->meshStep_->position_);
		}
		waypointUpdater_.updateWaypoints();
		if (waypointUpdater_.pathComplete())
		{
			setMode(search);
		}
		break;
	case search:
		steering_.updateSteering(currentTarget_);
		break;
	}

	if (vehicle_.getTransform()->position.y < -70)
	{
		resetFlag_ = true;
		physics_.respawnPlayer(id_);
	}
}

void NavigationSystem::setMode(NavMode mode)
{
	currentMode_ = mode;
}

float NavigationSystem::distanceFromVehicle(position target)
{
	return length(vehicle_.getTransform()->position - target);
}

bool NavigationSystem::hasPath()
{
	if (waypointUpdater_.pathComplete() && currentMode_ == nav)
	{
		return false;
	}
	return true;
}

void NavigationSystem::coolDownMagnet()
{
	magnetCooldown_ = glfwGetTime();
}

bool NavigationSystem::lostPath()
{
	return lostPath_;
}

bool NavigationSystem::queryReset()
{
	bool resetState = resetFlag_;
	if (resetFlag_)
	{
		resetFlag_ = false;
	}
	return resetState;
}