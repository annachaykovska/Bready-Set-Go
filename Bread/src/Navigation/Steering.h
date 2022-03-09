#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "../Scene/Entity.h"
#include "../Physics/PhysicsSystem.h"

typedef glm::vec3 position;

class Steering
{
public:
	Steering(Entity& entity, PhysicsSystem& physics, int id);

	void park();
	void updateSteering(position target);
	void setFinalApproach(bool status);

private:
	Entity& entity_;
	PhysicsSystem& physics_;
	physx::PxVehicleDrive4W* vehicle_;
	physx::PxVehicleDrive4WRawInputData* input_;
	int id_;
	bool finalApproach_;
};