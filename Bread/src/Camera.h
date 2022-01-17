#pragma once

#include <glm/glm.hpp>

class Camera
{
public:

	Camera(float theta, float phi, float radius);

	glm::mat4 getView();
	glm::vec3 getPos();
	void incrementTheta(float dt);
	void incrementPhi(float dp);
	void incrementR(float dr);

private:

	float theta;
	float phi;
	float radius;

	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
};