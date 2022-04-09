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
	a = 7.f;
	b = 4.6f;
	c = 0.01f;
	d = 1.1f;
}

void Camera::setProjMatrix()
{
	float width = g_systems.width;
	float height = g_systems.height;

	if (g_scene.numPlayers != 2)
		this->projMatrix = glm::perspective(glm::radians(this->perspective), width / height, 0.1f, 1000.0f);
	else
		this->projMatrix = glm::perspective(glm::radians(this->perspective), (width * 3 / 4) / (height / 2), 0.1f, 1000.0f);
}

float Camera::getPerspective()
{
	return perspective;
}

void Camera::setViewMatrix(int playerNum)
{
	float theta = 225.0f;
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
		float ratio = std::min(d * ((1 / (1 + pow(EULER, -a * (vehicleSpeed / TOP_SPEED) + b))) - c), 1.0f);
		perspective += ratio * 35.0f;
	}

	/*
	float max_fov_change = 2.f; //FIXME: THIS VARIABLE IS A CONSTANT 

	// Clamp FOV change to MAX_FOV_DELTA per frame
	if (this->perspective - this->oldFOV > max_fov_change)
		this->perspective = this->oldFOV + max_fov_change;
	else if (this->perspective - this->oldFOV < -max_fov_change)
		this->perspective = this->oldFOV - max_fov_change;
	*/

	float stopDegrees = 0.0f;

	// TODO Not sure what this is doing yet
	stopDegrees = abs(g_systems.physics->getViewDirectionalInfluence() * 90.0);
	if (g_systems.physics->getViewDirectionalInfluence() < 0)
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
	else if (g_systems.physics->getViewDirectionalInfluence() > 0)
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

	theta += cameraRotationOffset / 8 + this->forcedDirection;
	glm::mat4 rotation45 = glm::mat4(cos(glm::radians(theta)), 0, sin(glm::radians(theta)), 0,
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

	glm::vec4 positionFromVehicle = rotation45 * playerTransform->worldRotationMat * glm::vec4(1.0);
	float xChange = (CAMERA_DISTANCE + cameraPositionOffset) * positionFromVehicle.x;
	float zChange = (CAMERA_DISTANCE + cameraPositionOffset) * positionFromVehicle.z;

	position.x = playerTransform->position.x + xChange;
	position.y = playerTransform->position.y + CAMERA_GROUND_HEIGHT;
	position.z = playerTransform->position.z + zChange;

	glm::vec3 positionUp = glm::vec3(0.0f, 4.0f, 0.0f);
	glm::vec3 lookAhead = position + ((playerTransform->position + positionUp) - position) * 1.2f;
	centerBeam = lookAhead - position;
	this->lookAhead = lookAhead;
	
	this->viewMatrix = glm::lookAt(position, lookAhead, worldUp);
}

glm::mat4 Camera::getViewMatrix(Transform* playerTransform)
{
	float theta = 225.f;
	float cameraRotationOffset = 0.f;

	float elasticForce = 0.2f; // Lower = more resistance

	float vehicleSpeed = 0;
	float vehicleTurn = 0;

	float whiplashFOVChange=0;
	perspective = 40;
	cameraPositionOffset = 0;

	vehicleSpeed = g_systems.physics->mVehiclePlayer1->computeForwardSpeed();
	vehicleTurn = g_systems.physics->mVehiclePlayer1->computeSidewaysSpeed();

	if (vehicleSpeed > TOP_SPEED)
		vehicleSpeed = TOP_SPEED;

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
		
	float max_rot = 2.0f; //FIXME: THIS VARIABLE IS A CONSTANT

	if (!g_systems.physics->getIsVehicleInAir(1)) 
		cameraRotationOffset = vehicleSpeed * vehicleTurn / 1.5f;

	if (cameraRotationOffset - oldCameraRotationOffset > max_rot) 
		cameraRotationOffset = oldCameraRotationOffset + max_rot;
	else if (cameraRotationOffset - oldCameraRotationOffset < -max_rot) 
		cameraRotationOffset = oldCameraRotationOffset - max_rot;

	if (vehicleSpeed > 0)
	{
		float ratio = std::min(d * ((1 / (1 + pow(EULER, -a * (vehicleSpeed / TOP_SPEED) + b))) - c), 1.0f);
		perspective += ratio * 35.f;
		//cameraPositionOffset = (vehicleSpeed / 16);
	}

	float max_fov_change = 2.f; //FIXME: THIS VARIABLE IS A CONSTANT 

	if (perspective - oldFOV > max_fov_change) 
		perspective = oldFOV + max_fov_change;
	else if (perspective - oldFOV < -max_fov_change)
		perspective = oldFOV - max_fov_change;

	//perspective -= whiplashFOVChange;

	float stopDegrees = 0;

	stopDegrees = abs(g_systems.physics->getViewDirectionalInfluence() * 90.0);
	if (g_systems.physics->getViewDirectionalInfluence() < 0)
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
	else if (g_systems.physics->getViewDirectionalInfluence() > 0)
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

	// Setting old values
	lastSpeed = vehicleSpeed;
	oldForcedDirection = forcedDirection;
	oldCameraRotationOffset = cameraRotationOffset;
	oldFOV = perspective;

	theta += cameraRotationOffset / 8 + forcedDirection;
	glm::mat4 rotation45 = glm::mat4(cos(glm::radians(theta)), 0, sin(glm::radians(theta)), 0,
		0, 1.0f, 0, 0,
		-sin(glm::radians(theta)), 0, cos(glm::radians(theta)), 0,
		0, 0, 0, 1.0f);

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
	this->lookAhead = lookAhead;
	return glm::lookAt(position, lookAhead, worldUp);

	//Right
	//return glm::lookAt(glm::vec3(500.f, 10.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//Top-Down
	//return glm::lookAt(glm::vec3(-50.f, 900.0f, 200.0f), glm::vec3(-50.0f, 0.0f, 50.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::updateCameraPosition(const glm::vec3 pos)
{
	this->position.x = pos.x;
	this->position.y = pos.y;
	this->position.z = pos.z;
}

glm::mat4 Camera::recalculateViewMatrix()
{
	return glm::lookAt(position, lookAhead, worldUp);
}