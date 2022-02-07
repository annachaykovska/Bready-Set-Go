#pragma once

#include <iostream>

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <PxPhysicsAPI.h>

#include "Scene/Component.h"

class Transform : public Component
{

public:

	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	glm::vec3 heading;

	glm::mat4 model;

	Transform()
	{
		position = glm::vec3(0, 0, 0);
		rotation = glm::vec3(0, 0, 0);
		scale = glm::vec3(1, 1, 1);

		model = glm::mat4(1.f);
	}

	glm::mat4 getModelMatrix()
	{
		return model;
	}

	void update(physx::PxTransform transform)
	{
		this->position.x = transform.p.x;
		this->position.y = transform.p.y;
		this->position.z = transform.p.z;

		glm::mat4 translationMatrix = glm::translate(glm::mat4(1.f), this->position);

		glm::quat newQuat = glm::quat(transform.q.w, transform.q.x, transform.q.y, transform.q.z);
		glm::mat4 rotationMatrix = toMat4(newQuat);

		glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.f), this->scale);

		this->heading = rotationMatrix * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f);
		
		this->model = translationMatrix * rotationMatrix * scaleMatrix;
	}
};