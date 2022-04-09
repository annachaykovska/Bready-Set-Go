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
	glm::vec3 centerBeam;

	glm::mat4 viewMatrix;
	glm::mat4 projMatrix;

	Camera();
	glm::mat4 getViewMatrix(Transform* playerTransform);

	void updateCameraPosition(const glm::vec3 pos);
	glm::mat4 recalculateViewMatrix();

	void setProjMatrix();
	void setViewMatrix(int playerNum);

	float getPerspective();

private:

	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;
	glm::vec3 lookAhead;

	float perspective;
	float yaw;
	float pitch;

	float lastOffset;
	float lastSpeed;

	float slowDownCounter;
	int counter;

	float cameraPositionOffset;
	float forcedDirection;
	float recordedForcedDirection;
	float oldForcedDirection;
	float oldCameraRotationOffset;
	float oldFOV;

	float predictedCameraDelta;

	float movementSpeed;
	float mouseSensitivity;
	float zoom;

	// Camera Tweaks
	bool in_whiplash;
	int total_whip_lash_frames;
	int whiplash_frame;

	//Tweakable
	float max_fov;

	// values for the fov sigmoid function
	float a;
	float b;
	float c;
	float d;
};