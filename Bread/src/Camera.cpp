#define _USE_MATH_DEFINES
#include <math.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Camera.h"

#define CAMERA_DISTANCE 50.0

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
	Transform transform = Transform();
	transform.position = glm::vec3(1.0f);
	updateCameraVectors(&transform);
}

glm::mat4 Camera::getViewMatrix(Transform* playerTransform)
{
	/*glm::vec3 offset = glm::vec3(0, 3, 20);
	glm::mat4 cp = glm::rotate(glm::mat4(1.0f), float(yaw), glm::vec3(0, 1, 0));
	this->position = playerTransform->position * cp + offset;*/
	/*glm::vec3 positionFromParent = {
				CAMERA_DISTANCE * sin(playerTransform->rotation.x),
				CAMERA_DISTANCE * sin(playerTransform->rotation.y) * sin(playerTransform->rotation.z) + 20,
				CAMERA_DISTANCE * cos(playerTransform->rotation.x)
	};
	position = playerTransform->position;*/
	glm::mat4 rotation45 = glm::mat4(cos(45), 0, sin(45), 0,
		0, 1.0f, 0, 0,
		-sin(45), 0, cos(45), 0,
		0, 0, 0, 1.0f);;
	glm::vec4 positionFromVehicle = rotation45 * playerTransform->rotationMat * glm::vec4(1.0);
	float xChange = CAMERA_DISTANCE * positionFromVehicle.x;
	float zChange = CAMERA_DISTANCE * positionFromVehicle.z;

	position.x = playerTransform->position.x + xChange;
	position.y = playerTransform->position.y + 40.0f;
	position.z = playerTransform->position.z + zChange;
	return glm::lookAt(position, playerTransform->position, up);
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