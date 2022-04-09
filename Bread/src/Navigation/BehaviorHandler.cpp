#include "BehaviorHandler.h"

BehaviorHandler::BehaviorHandler(Transform* vehicleTransform) 
: currentState(normal)
, transform(vehicleTransform)
{
}

void BehaviorHandler::update(float currentSpeed)
{
	float currentTime = glfwGetTime();

	if (currentSpeed > 5.f)
	{
		currentTime = glfwGetTime();
		positionDelta = transform->position;
	}
	else if((currentTime - stuckTimer) > 5)
	{
		float travelDistance = length(transform->position - positionDelta);
		if (travelDistance < 5.f)
		{
			setState(unsticking);
			stuckPosition = transform->position;
		}
	}
}

bool BehaviorHandler::checkUnstuck()
{
	float travelDistance = length(transform->position - stuckPosition);
	if (travelDistance > 8)
	{
		return true;
	}
	return false;
}

void BehaviorHandler::setState(DriveState state)
{
	currentState = state;
}

DriveState BehaviorHandler::getState()
{
	return currentState;
}