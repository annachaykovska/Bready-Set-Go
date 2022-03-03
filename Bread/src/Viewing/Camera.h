#pragma once

#include <string>
#include <glm/glm.hpp>

#include "../Transform.h"


const float YAW = -90.0f;
const float PITCH = -30.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 35.0f;

class Camera
{
public:
	glm::vec3 position;
	float yaw;
	float pitch;

	Camera();
	glm::mat4 getViewMatrix(Transform* playerTransform);
	void updateCameraVectors(Transform* playerTransform);

private:

	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;

	float movementSpeed;
	float mouseSensitivity;
	float zoom;
};