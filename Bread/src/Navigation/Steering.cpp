#include "Steering.h"

namespace
{
	const float TURN_THRESHOLD = 0.2f;
	const double PI = 3.1415926535;
}

Steering::Steering(Entity& entity, PhysicsSystem& physics, int id)
	: entity_(entity)
	, physics_(physics)
	, id_(id)
	, finalApproach_(false)
{
	switch (id_)
	{
	case 1:
		vehicle_ = physics_.mVehiclePlayer1;
		input_ = &physics_.mVehicleInputDataPlayer1;
		break;
	case 2:
		vehicle_ = physics_.mVehiclePlayer2;
		input_ = &physics_.mVehicleInputDataPlayer2;
		break;
	case 3:
		vehicle_ = physics_.mVehiclePlayer3;
		input_ = &physics_.mVehicleInputDataPlayer3;
		break;
	case 4:
		vehicle_ = physics_.mVehiclePlayer4;
		input_ =&physics_.mVehicleInputDataPlayer4;
		break;
	}
}

void Steering::park()
{
	vehicle_->mWheelsDynData.setToRestState();
}

void Steering::updateSteering(position target)
{
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

	int max = 25;
	if (vehicle_->computeForwardSpeed() < max)
	{
		input_->setAnalogAccel(1.f);
	}
	else
	{
		input_->setAnalogAccel(0.f);
	}
	//if (abs(theta) > TURN_THRESHOLD)
	//{
		/*
		if (abs(theta) > PI / 4.0f)
		{
			theta = PI / 4.0f;
		}
		*/
		if (upVector.y < 0)
		{
			//input_->setAnalogSteer(theta / (PI / 4.0f));
			input_->setAnalogSteer(1.0f);
		}
		else
		{
			//input_->setAnalogSteer(-theta / (PI / 4.0f));
			input_->setAnalogSteer(-1.0f);
		}
	//}
	//else
	//{
	//	std::cout << "Shouldn't stay here long" << std::endl;
	//	input_->setAnalogSteer(0.f);
	//}
}