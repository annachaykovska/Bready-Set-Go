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
	, currentTime_(0)
	, stuckTimer_(0)
	, stuck_(false)
	, pause_(false)
	, procedureTimer_(0)
	, positionDelta_(glm::vec3(0, 0, 0))
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

void Steering::evasiveAction()
{
	vehicle_->mDriveDynData.forceGearChange(snippetvehicle::PxVehicleGearsData::eREVERSE);
	input_->setAnalogAccel(0.3f);
	if (currentTime_ - procedureTimer_ > 2)
	{
		stuck_ = false;
		stuckTimer_ = glfwGetTime();
		procedureTimer_ = glfwGetTime();
	}
}

void Steering::brakeAction()
{
	if (currentTime_ - procedureTimer_ <= 0.5)
	{
		input_->setAnalogAccel(0.f);
		input_->setAnalogBrake(1.f);
	}
	else if (currentTime_ - procedureTimer_ > 0.5 && currentTime_ - procedureTimer_ < 1)
	{
		vehicle_->mDriveDynData.forceGearChange(snippetvehicle::PxVehicleGearsData::eREVERSE);
		input_->setAnalogBrake(0.f);
		input_->setAnalogAccel(0.3f);
		input_->setAnalogSteer(1.0f);
	}
	else
	{
		pause_ = false;
		procedureTimer_ = glfwGetTime();
		input_->setAnalogBrake(0.f);
	}
}

void Steering::pause()
{
	pause_ = true;
	procedureTimer_ = glfwGetTime();
}

bool Steering::locked()
{
	return false;
}

void Steering::updateSteering(position target)
{
	currentTime_ = glfwGetTime();

	if (pause_)
	{
		std::cout << "Braking" << std::endl;
		brakeAction();
		return;
	}

	if (stuck_)
	{
		std::cout << "Unstucking" << std::endl;
		evasiveAction();
		return;
	}

	std::cout << "Steering" << std::endl;
	vehicle_->mDriveDynData.forceGearChange(snippetvehicle::PxVehicleGearsData::eFIRST);
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

	if (vehicle_->computeForwardSpeed() > 5)
	{
		stuckTimer_ = glfwGetTime();
	}

	if (currentTime_ - stuckTimer_ > 2)
	{
		stuckTimer_ = glfwGetTime();
		stuck_ = true;
		procedureTimer_ = glfwGetTime();
	}

	int max = 25;
	if (vehicle_->computeForwardSpeed() < max)
	{
		input_->setAnalogAccel(1.f);
	}
	else
	{
		input_->setAnalogAccel(0.f);
	}
	if (upVector.y < 0)
	{
		input_->setAnalogSteer(1.0f);
	}
	else
	{
		input_->setAnalogSteer(-1.0f);
	}
}