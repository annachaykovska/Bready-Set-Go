#pragma once
#include "../Scene/Entity.h"

enum DriveState {normal, careful, intercepting, unsticking, eavding};

class BehaviorHandler
{
public:
	BehaviorHandler(Transform* vehicleTransform);

	void update(float currentSpeed);

	bool checkUnstuck();

	void setState(DriveState state);
	DriveState getState();

private:
	DriveState currentState;
	Transform* transform;
	
	float stuckTimer;
	glm::vec3 positionDelta;
	glm::vec3 stuckPosition;
};