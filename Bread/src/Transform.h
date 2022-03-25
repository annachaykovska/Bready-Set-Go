#pragma once

#include <glm/glm.hpp>
#include <PxPhysicsAPI.h>

#include "Scene/Component.h"

class Transform : public Component
{

public:

	glm::vec3 position;
	glm::vec3 rotation;
	glm::mat4 rotationMat;
	glm::mat4 worldRotationMat;
	glm::vec3 scale;
	glm::vec3 heading;
	glm::mat4 model;

	Transform();

	glm::mat4 getModelMatrix() { return this->model; }

	void update();
	void update(physx::PxTransform transform);
};