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
	void evasiveAction();
	void brakeAction();
	void pause();
	bool locked();

private:
	Entity& entity_;
	PhysicsSystem& physics_;
	physx::PxVehicleDrive4W* vehicle_;
	physx::PxVehicleDrive4WRawInputData* input_;
	int id_;
	bool finalApproach_;

	int stuckTimer_;
	bool stuck_;
	bool locked_;
	bool pause_;

	int deltaTimer_;
	position positionDelta_;

	int procedureTimer_;
};