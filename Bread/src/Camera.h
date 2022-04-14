#pragma once

#include <string>
#include <glm/glm.hpp>

class Transform;

class Camera
{
public:

	glm::vec3 position;
	glm::vec3 centerBeam;

	glm::mat4 viewMatrix;
	glm::mat4 projMatrix;

	float viewDirectionalInfluence;
	float turnDirectionalInfluence;

	Camera();

	void update(const int playerNum);
	void recalculateViewMatrix(const glm::vec3 newPos);
	float getPerspective();

private:

	const glm::vec3 worldUp = glm::vec3(0, 1.0f, 0);

	glm::vec3 lookAhead;

	float perspective;
	float lastSpeed;
	float cameraPositionOffset;
	float forcedDirection;
	float recordedForcedDirection;
	float oldForcedDirection;
	float oldCameraRotationOffset;
	float oldFOV;

	float prevTime;

	void setProjMatrix();
	void setViewMatrix(const int playerNum);
};