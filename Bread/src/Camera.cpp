#define _USE_MATH_DEFINES
#include <math.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Camera.h"

#define CAMERA_DISTANCE 10.0
#define CAMERA_GROUND_HEIGHT 5.0

Camera::Camera()
{
	this->position = glm::vec3(0, 200.0f, 500.0f);
	this->front = glm::vec3(0, 0, -1.f);
	this->up = glm::vec3(0, 1.f, 0);
	this->worldUp = glm::vec3(0, 1.f, 0);	
	this->yaw = YAW;
	this->pitch = PITCH;
	this->movementSpeed = SPEED;
	this->mouseSensitivity = SENSITIVITY;
	this->zoom = ZOOM; 
	this->lastOffset = 0;
	this->lastSpeed = 0;
	this->cameraPositionOffset = 0;
	this->slowDownCounter = 0;
	this->perspective = 40;
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
	glm::vec3 defaultPosition;

	float theta = 225.f;
	float cameraRotationOffset = 0.f;

	float vehicleSpeed = 0;
	float vehicleTurn = 0;

	perspective = 40;
	cameraPositionOffset = 0;

	if (physics != nullptr)
	{
		vehicleSpeed = physics->mVehiclePlayer1->computeForwardSpeed();
		vehicleTurn = physics->mVehiclePlayer1->computeSidewaysSpeed();

		cameraRotationOffset = vehicleSpeed * vehicleTurn;
	}

	if (vehicleSpeed > 0)
	{
		perspective = 40 + (vehicleSpeed / 12);
		cameraPositionOffset = vehicleSpeed / 16;
	}

	glm::mat4 rotation45 = glm::mat4(cos(glm::radians(theta)), 0, sin(glm::radians(theta)), 0,
		0, 1.0f, 0, 0,
		-sin(glm::radians(theta)), 0, cos(glm::radians(theta)), 0,
		0, 0, 0, 1.0f);;
	glm::vec4 positionFromVehicle = rotation45 * playerTransform->rotationMat * glm::vec4(1.0);
	float xChange = (CAMERA_DISTANCE + cameraPositionOffset) * positionFromVehicle.x;
	float zChange = (CAMERA_DISTANCE + cameraPositionOffset) * positionFromVehicle.z;

	defaultPosition.x = playerTransform->position.x + xChange;
	defaultPosition.y = playerTransform->position.y + CAMERA_GROUND_HEIGHT;
	defaultPosition.z = playerTransform->position.z + zChange;

	theta += cameraRotationOffset / 16;
	rotation45 = glm::mat4(cos(glm::radians(theta)), 0, sin(glm::radians(theta)), 0,
		0, 1.0f, 0, 0,
		-sin(glm::radians(theta)), 0, cos(glm::radians(theta)), 0,
		0, 0, 0, 1.0f);;
	positionFromVehicle = rotation45 * playerTransform->rotationMat * glm::vec4(1.0);
	xChange = (CAMERA_DISTANCE + cameraPositionOffset) * positionFromVehicle.x;
	zChange = (CAMERA_DISTANCE + cameraPositionOffset) * positionFromVehicle.z;

	position.x = playerTransform->position.x + xChange;
	position.y = playerTransform->position.y + CAMERA_GROUND_HEIGHT;
	position.z = playerTransform->position.z + zChange;

	glm::vec3 positionUp = glm::vec3(0.0f, 4.0f, 0.0f);
	glm::vec3 lookAhead = defaultPosition + ((playerTransform->position + positionUp) - defaultPosition) * 1.2f;
	//return glm::lookAt(position, lookAhead, worldUp);
	return glm::lookAt(glm::vec3(30.f, 150.0f, 50.0f), glm::vec3(30.0f, 0.0f, -30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::updateCameraVectors(Transform* playerTransform)
{
	// Calculate new front vector using Euler angles
	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	this->front = glm::normalize(front);

	// Calculate new right and up vectors using cross product
	this->right = glm::normalize(glm::cross(front, worldUp));
	this->up = glm::normalize(glm::cross(right, front));
}

void Camera::initPhysics(PhysicsSystem* physicsSystem)
{
	physics = physicsSystem;
}