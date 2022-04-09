#define _USE_MATH_DEFINES
#include <math.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Camera.h"
#include "SystemManager.h"
#include "Transform.h"
#include "Physics/PhysicsSystem.h"
#include "Scene/Scene.h"
#include <glm/gtc/type_ptr.hpp>

#define CAMERA_DISTANCE 12.0
#define CAMERA_GROUND_HEIGHT 5.0
#define TOP_SPEED 40.0f
#define MAX_ROTATION 2.0f
#define MAX_FOV_DELTA 2.0f
#define ELASTIC_FORCE 0.2f
#define EULER 2.71828f

extern SystemManager g_systems;
extern Scene g_scene;

Camera::Camera()
	: position(glm::vec3(0.0f, 200.0f, 500.0f))
	, projMatrix(glm::mat4(1.0f))
	, viewMatrix(glm::mat4(1.0f))
	, lastSpeed(0.0f)
	, cameraPositionOffset(0.0f)
	, perspective(40.f)
	, forcedDirection(0.0f)
	, oldForcedDirection(forcedDirection)
	, recordedForcedDirection(forcedDirection)
	, oldCameraRotationOffset(0.0f)
	, oldFOV(40.0f)
{}

void Camera::update(const int playerNum)
{
	setProjMatrix();
	setViewMatrix(playerNum);
}

void Camera::setProjMatrix()
{
	float width = g_systems.width;
	float height = g_systems.height;

	switch (g_scene.numPlayers)
	{
	case 1:
		this->projMatrix = glm::perspective(glm::radians(this->perspective), width / height, 0.1f, 1000.0f);
		break;
	case 2:
		this->projMatrix = glm::perspective(glm::radians(this->perspective), (width * 0.75f) / (height / 2.0f), 0.1f, 1000.0f);
		break;
	case 3:
		this->projMatrix = glm::perspective(glm::radians(this->perspective), (width / 2.0f) / (height / 2.0f), 0.1f, 1000.0f);
		break;
	case 4:
		this->projMatrix = glm::perspective(glm::radians(this->perspective), (width / 2.0f) / (height / 2.0f), 0.1f, 1000.0f);
		break;
	default:
		return; // Uh-oh, something has gone wrong
		break;
	}
}

void Camera::setViewMatrix(const int playerNum)
{
	float cameraRotationOffset = 0.0f;

	this->cameraPositionOffset = 0.0f;
	this->perspective = 40.0f;

	float vehicleSpeed = g_systems.physics->getPlayerSpeed(playerNum);
	float vehicleTurn = g_systems.physics->getPlayerSidewaysSpeed(playerNum);

	// Clamp vehicle's speed to TOP_SPEED for FOV changes
	if (vehicleSpeed > TOP_SPEED)
		vehicleSpeed = TOP_SPEED;

	if (!g_systems.physics->getIsVehicleInAir(playerNum))
		cameraRotationOffset = vehicleSpeed * vehicleTurn / 1.5f;

	// Clamp camera rotation to MAX_ROTATION
	if (cameraRotationOffset - oldCameraRotationOffset > MAX_ROTATION)
		cameraRotationOffset = oldCameraRotationOffset + MAX_ROTATION;
	else if (cameraRotationOffset - oldCameraRotationOffset < -MAX_ROTATION)
		cameraRotationOffset = oldCameraRotationOffset - MAX_ROTATION;

	// Change FOV based on sigmoid function wrt the vehicle's speed
	if (vehicleSpeed > 0)
	{
		float ratio = std::min(1.1f * ((1 / (1 + pow(EULER, -7.0f * (vehicleSpeed / TOP_SPEED) + 4.6f))) - 0.01f), 1.0f);
		perspective += ratio * 35.0f;
	}

	// Clamp FOV change to MAX_FOV_DELTA per frame
	if (this->perspective - this->oldFOV > MAX_FOV_DELTA)
		this->perspective = this->oldFOV + MAX_FOV_DELTA;
	else if (this->perspective - this->oldFOV < -MAX_FOV_DELTA)
		this->perspective = this->oldFOV - MAX_FOV_DELTA;

	float stopDegrees = abs(this->viewDirectionalInfluence * 90.0);

	if (this->viewDirectionalInfluence < 0)
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
				float functionStep = (ELASTIC_FORCE / stopDegrees) * abs(forcedDirection);
				functionStep = ELASTIC_FORCE - functionStep;
				stepSize = -(1 / (5 * (functionStep + 0.2))) + 1;
			}
			if (forcedDirection + stepSize < stopDegrees)
			{
				forcedDirection += stepSize;
			}
		}
		else
		{
			float functionStep = (ELASTIC_FORCE / stopDegrees) * abs(forcedDirection);
			functionStep -= ELASTIC_FORCE;
			stepSize = -(1 / (5 * (functionStep + 0.2))) + 1;
			if (forcedDirection - stepSize > stopDegrees)
			{
				forcedDirection -= stepSize;
			}
		}
		recordedForcedDirection = forcedDirection;
	}
	else if (this->viewDirectionalInfluence > 0)
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
				float functionStep = (ELASTIC_FORCE / stopDegrees) * abs(forcedDirection);
				functionStep = ELASTIC_FORCE - functionStep;
				stepSize = -(1 / (5 * (functionStep + 0.2))) + 1;
			}
			if (forcedDirection > -stopDegrees)
			{
				forcedDirection -= stepSize;
			}
		}
		else
		{
			float functionStep = (ELASTIC_FORCE / stopDegrees) * abs(forcedDirection);
			functionStep -= ELASTIC_FORCE;
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
	
	// Save this frame's values for next frame
	this->lastSpeed = vehicleSpeed;
	this->oldForcedDirection = this->forcedDirection;
	this->oldCameraRotationOffset = cameraRotationOffset;
	this->oldFOV = this->perspective;

	float theta = 225.0f;
	theta += cameraRotationOffset / 8 + this->forcedDirection;
	glm::mat4 rotation = glm::mat4(cos(glm::radians(theta)), 0, sin(glm::radians(theta)), 0,
									 0, 1.0f, 0, 0,
									-sin(glm::radians(theta)), 0, cos(glm::radians(theta)), 0,
									 0, 0, 0, 1.0f);

	// Get the player's transform for this camera
	Transform* playerTransform;
	switch (playerNum)
	{
	case 1:
		playerTransform = g_scene.getEntity("player1")->getTransform();
		break;
	case 2:
		playerTransform = g_scene.getEntity("player2")->getTransform();
		break;
	case 3:
		playerTransform = g_scene.getEntity("player3")->getTransform();
		break;
	case 4:
		playerTransform = g_scene.getEntity("player4")->getTransform();
		break;
	default:
		return; // Uh-oh, something has gone wrong
		break;
	}

	glm::vec4 positionFromVehicle = rotation * playerTransform->worldRotationMat * glm::vec4(1.0);
	float xChange = (CAMERA_DISTANCE + cameraPositionOffset) * positionFromVehicle.x;
	float zChange = (CAMERA_DISTANCE + cameraPositionOffset) * positionFromVehicle.z;

	position.x = playerTransform->position.x + xChange;
	position.y = playerTransform->position.y + CAMERA_GROUND_HEIGHT;
	position.z = playerTransform->position.z + zChange;

	glm::vec3 positionUp = glm::vec3(0.0f, 4.0f, 0.0f);
	glm::vec3 lookAhead = position + ((playerTransform->position + positionUp) - position) * 1.2f;
	
	this->viewMatrix = glm::lookAt(position, lookAhead, worldUp);
}