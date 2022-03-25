#define _USE_MATH_DEFINES
#include <math.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Camera.h"

#define CAMERA_DISTANCE 10.0
#define CAMERA_GROUND_HEIGHT 5.0
#define TOP_SPEED 40.f

Camera::Camera()
	: position(glm::vec3(0, 200.0f, 500.0f))
	, front(glm::vec3(0, 0, -1.f))
	, up(glm::vec3(0, 1.f, 0))
	, worldUp(glm::vec3(0, 1.f, 0))
	, cameraPositionOffset(0.f)
	, perspective(40.f)
	, forcedDirection(0.f)
	, oldForcedDirection(forcedDirection)
	, recordedForcedDirection(forcedDirection)
	, counter(0)
	, oldCameraRotationOffset(0.f)
	, predictedCameraDelta(0.f)
	, max_fov(50.f)
	, in_whiplash(false)
{
	Transform transform = Transform();
	transform.position = glm::vec3(1.0f);
	updateCameraVectors(&transform);
}

float Camera::getPerspective()
{
	return perspective;
}

glm::mat4 Camera::getViewMatrix(Transform* playerTransform)
{
	float theta = 225.f;
	float cameraRotationOffset = 0.f;

	float elasticForce = 0.8f; // Lower = more resistance

	float vehicleSpeed = 0;
	float vehicleTurn = 0;

	float whiplashFOVChange=0;
	perspective = 40;
	cameraPositionOffset = 0;

	if (physics != nullptr)
	{
		vehicleSpeed = physics->mVehiclePlayer1->computeForwardSpeed();
		vehicleTurn = physics->mVehiclePlayer1->computeSidewaysSpeed();

		if (vehicleSpeed > TOP_SPEED)
		{
			vehicleSpeed = TOP_SPEED;
			//std::cout << "top speed" << std::endl;
		}
		else {
			//std::cout << "not top speed" << std::endl;
		}

		//TODO: FIX (whiplash happens back to back)
		// See if harsh stop happened
		//if (((vehicleSpeed > 0 && vehicleSpeed - lastSpeed < -0.1f) || (vehicleSpeed < 0 && vehicleSpeed - lastSpeed > 0.1f)) && !in_whiplash) {
		//	in_whiplash = true;
		//	total_whip_lash_frames=10;
		//	whiplash_frame = 0;
		//}
		//if (in_whiplash) {
		//	float x = (float)(whiplash_frame++)/total_whip_lash_frames;
		//	std::cout << x << std::endl;
		//	whiplashFOVChange = 4.f*(x)*(x - 1.f)*20.f;
		//	if (whiplash_frame == total_whip_lash_frames) in_whiplash = false;
		//}
		
		float max_rot=8.f; //FIXME: THIS VARIABLE IS A CONSTANT
		cameraRotationOffset = vehicleSpeed * vehicleTurn * 2.5;
		if (cameraRotationOffset - oldCameraRotationOffset > max_rot) cameraRotationOffset = oldCameraRotationOffset + max_rot;
		else if (cameraRotationOffset - oldCameraRotationOffset < -max_rot) cameraRotationOffset = oldCameraRotationOffset - max_rot;
	}
	if (vehicleSpeed > 0)
	{
		perspective += powf((vehicleSpeed / TOP_SPEED), 2.f) * 60.f;
		//cameraPositionOffset = (vehicleSpeed / 16);
	}
	float max_fov_change = 4.f; //FIXME: THIS VARIABLE IS A CONSTANT 
	if (perspective - oldFOV > max_fov_change) {
		perspective = oldFOV + max_fov_change;
		std::cout << "FOV CHANGE TOO HIGH" << std::endl;
	}
	else if (perspective - oldFOV < -max_fov_change) {
		perspective = oldFOV - max_fov_change;
		std::cout << "FOV CHANGE TOO LOW" << std::endl;
	}

	//perspective -= whiplashFOVChange;

	float stopDegrees = 0;
	if (physics != nullptr)
	{
		stopDegrees = abs(physics->getViewDirectionalInfluence() * 90.0);
		if (physics->getViewDirectionalInfluence() < 0)
		{
			float stepSize = 0;
			if (forcedDirection < stopDegrees)
			{
				if (forcedDirection < 0)
				{
					stepSize = -(1 / (5 * 2.f)) + 1;
				}
				else
				{
					float functionStep = (elasticForce / stopDegrees) * abs(forcedDirection);
					functionStep = elasticForce - functionStep;
					stepSize = -(1 / (5 * (functionStep + 0.2))) + 1;
				}
				if (forcedDirection + stepSize < stopDegrees)
				{
					forcedDirection += stepSize;
				}
			}
			else
			{
				float functionStep = (elasticForce / stopDegrees) * abs(forcedDirection);
				functionStep -= elasticForce;
				stepSize = -(1 / (5 * (functionStep + 0.2))) + 1;
				if (forcedDirection - stepSize > stopDegrees)
				{
					forcedDirection -= stepSize;
				}
			}
			recordedForcedDirection = forcedDirection;
		}
		else if (physics->getViewDirectionalInfluence() > 0)
		{
			float stepSize = 0;
			if (abs(forcedDirection) < stopDegrees)
			{
				if (forcedDirection > 0)
				{
					stepSize = -(1 / (5 * 2.f)) + 1;
				}
				else
				{
					float functionStep = (elasticForce / stopDegrees) * abs(forcedDirection);
					functionStep = elasticForce - functionStep;
					stepSize = -(1 / (5 * (functionStep + 0.2))) + 1;
				}
				if (forcedDirection > -stopDegrees)
				{
					forcedDirection -= stepSize;
				}
			}
			else
			{
				float functionStep = (elasticForce / stopDegrees) * abs(forcedDirection);
				functionStep -= elasticForce;
				stepSize = -(1 / (5 * (functionStep + 0.2))) + 1;
				if (forcedDirection + stepSize < stopDegrees)
				{
					forcedDirection += stepSize;
				}
			}
			recordedForcedDirection = forcedDirection;
		}
		else
		{
			if (abs(forcedDirection) > 0.001)
			{
				float functionStep = (1.2f / abs(recordedForcedDirection)) * abs(forcedDirection);
				float stepSize = -(1 / (5 * (functionStep + 0.2))) + 1;
				if (forcedDirection > 0)
				{
					if (forcedDirection - stepSize < 0)
					{
						forcedDirection = 0;
					}
					else
					{
						forcedDirection -= stepSize;
					}
				}
				else
				{
					if (forcedDirection + stepSize > 0)
					{
						forcedDirection = 0;
					}
					else
					{
						forcedDirection += stepSize;
					}
				}
			}
			else
			{
				forcedDirection = 0;
			}
		}
	}
	// Setting old values
	lastSpeed = vehicleSpeed;
	oldForcedDirection = forcedDirection;
	oldCameraRotationOffset = cameraRotationOffset;
	oldFOV = perspective;

	theta += cameraRotationOffset / 8 + forcedDirection;
	glm::mat4 rotation45 = glm::mat4(cos(glm::radians(theta)), 0, sin(glm::radians(theta)), 0,
		0, 1.0f, 0, 0,
		-sin(glm::radians(theta)), 0, cos(glm::radians(theta)), 0,
		0, 0, 0, 1.0f);;
	//glm::vec4 positionFromVehicle = rotation45 * playerTransform->rotationMat * glm::vec4(1.0);
	glm::vec4 positionFromVehicle = rotation45 * playerTransform->worldRotationMat * glm::vec4(1.0);
	float xChange = (CAMERA_DISTANCE + cameraPositionOffset) * positionFromVehicle.x;
	float zChange = (CAMERA_DISTANCE + cameraPositionOffset) * positionFromVehicle.z;

	position.x = playerTransform->position.x + xChange;
	position.y = playerTransform->position.y + CAMERA_GROUND_HEIGHT;
	position.z = playerTransform->position.z + zChange;

	glm::vec3 positionUp = glm::vec3(0.0f, 4.0f, 0.0f);
	glm::vec3 lookAhead = position + ((playerTransform->position + positionUp) - position) * 1.2f;
	centerBeam = lookAhead - position;
	return glm::lookAt(position, lookAhead, worldUp);

	//Right
	//return glm::lookAt(glm::vec3(500.f, 10.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//Top-Down
	//return glm::lookAt(glm::vec3(-50.f, 900.0f, 200.0f), glm::vec3(-50.0f, 0.0f, 50.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::updateCameraVectors(Transform* playerTransform)
{
	/*
	// Calculate new front vector using Euler angles
	glm::vec3 front = playerTransform->heading;
	front.x = cos(glm::radians(YAW)) * cos(glm::radians(PITCH));
	front.y = sin(glm::radians(PITCH));
	front.z = sin(glm::radians(YAW)) * cos(glm::radians(PITCH));
	this->front = glm::normalize(front);

	// Calculate new right and up vectors using cross product
	this->right = glm::normalize(glm::cross(front, worldUp));
	this->up = glm::normalize(glm::cross(right, front));
	*/
}

void Camera::initPhysics(PhysicsSystem* physicsSystem)
{
	physics = physicsSystem;
}