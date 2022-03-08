#include "Steering.h"

namespace
{
	const float TURN_THRESHOLD = 0.2f;
	const double PI = 3.1415926535;
}

Steering::Steering(Entity& entity, PhysicsSystem& physics)
	: entity_(entity)
	, physics_(physics)
{
}

void Steering::updateSteering(position target)
{
	//std::cout << target.x << ", " << target.y << ", " << target.z << std::endl;
	glm::vec3 vectorA = target - entity_.getTransform()->position;
	float distance = length(vectorA);
	distance /= 20;
	if (distance > 1)
	{
		distance = 1;
	}
	vectorA /= length(vectorA);
	glm::vec3 vectorB = entity_.getTransform()->heading;
	
	float theta = acos(dot(vectorB, vectorA) / (length(vectorA) * length(vectorB)));
	glm::vec3 upVector = cross(vectorB, vectorA);

	if (abs(theta) > TURN_THRESHOLD)
	{
		if (upVector.y < 0)
		{
			//physics_.mVehicleInputData.setAnalogSteer(1.f);
		}
		else
		{
			//physics_.mVehicleInputData.setAnalogSteer(-1.f);
		}
	}
	else
	{
		//physics_.mVehicleInputData.setAnalogSteer(0.f);
	}
}