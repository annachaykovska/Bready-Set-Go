#pragma once

#include <string>
#include <glm/glm.hpp>

class Transform;

const float YAW = -90.0f;
const float PITCH = -30.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 35.0f;

class Camera
{
public:

	glm::vec3 position;

	glm::mat4 viewMatrix;
	glm::mat4 projMatrix;

	float viewDirectionalInfluence;
	float turnDirectionalInfluence;

	Camera();

	void update(const int playerNum);

private:

	const glm::vec3 worldUp = glm::vec3(0, 1.0f, 0);

	float perspective;
	float lastSpeed;
	float cameraPositionOffset;
	float forcedDirection;
	float recordedForcedDirection;
	float oldForcedDirection;
	float oldCameraRotationOffset;
	float oldFOV;

	void setProjMatrix();
	void setViewMatrix(const int playerNum);
};