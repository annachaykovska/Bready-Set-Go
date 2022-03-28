#include "NavigationSystem.h"

NavigationSystem::NavigationSystem(Entity& vehicle, PhysicsSystem& physics, NavMesh& navmesh, int id)
	: vehicle_(vehicle)
	, physics_(physics)
	, navmesh_(navmesh)
	, pathFinder_(navmesh_)
	, id_(id)
	, steering_(vehicle_, physics_, id_)
	, waypointUpdater_(vehicle_, navmesh_)
	, resetFlag_(false)
	, currentMode_(nav)
	, magnetCooldown_(0)
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

void NavigationSystem::pause()
{
	steering_.park();
}

void NavigationSystem::update()
{
	if (magnetCooldown_ > 0)
	{
		magnetCooldown_--;
	}
	if (length(currentTarget_ - vehicle_.getTransform()->position) < 20)
	{
		if (magnetCooldown_ < 1)
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
			if (steering_.locked())
			{
				std::cout << "Respawning tipped player" << std::endl;
				physics_.respawnPlayer(id_);
			}
			else
			{
				steering_.updateSteering(waypointUpdater_.currentWaypoint()->meshStep_->position_);
			}
		}
		waypointUpdater_.updateWaypoints();
		if (waypointUpdater_.pathComplete())
		{
			currentMode_ = search;
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
	magnetCooldown_ = 400;
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