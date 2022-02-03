#define _USE_MATH_DEFINES
#include <math.h>

#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"

Camera::Camera(float theta, float phi, float radius)
{
	this->theta = theta;
	this->phi = phi;
	this->radius = radius;

	this->cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	this->cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	this->cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
}

glm::mat4 Camera::getView()
{
	glm::vec3 cameraPosition = radius * glm::vec3(std::cos(theta) * std::sin(phi), std::sin(theta), std::cos(theta) * std::cos(phi));
	glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	return glm::lookAt(cameraPosition, cameraDirection, cameraUp);
}

glm::vec3 Camera::getPos() 
{
	return radius * glm::vec3(std::cos(theta) * std::sin(phi), std::sin(theta), std::cos(theta) * std::cos(phi));
}

void Camera::incrementTheta(float dt) 
{
	if (theta + (dt / 100.0f) < M_PI_2 && theta + (dt / 100.0f) > -M_PI_2) 
	{
		theta += dt / 100.0f;
	}
}

void Camera::incrementPhi(float dp) 
{
	phi -= dp / 100.0f;

	if (phi > 2.0 * M_PI) {
		phi -= 2.0 * M_PI;
	}
	else if (phi < 0.0f) {
		phi += 2.0 * M_PI;
	}
}

void Camera::incrementR(float dr) 
{
	radius -= dr;
}