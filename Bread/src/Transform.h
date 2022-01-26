#pragma once

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "Scene/Component.h"

class Transform : public Component
{

public:

	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	Transform()
	{
		position = glm::vec3(0, 0, 0);
		rotation = glm::vec3(0, 0, 0);
		scale = glm::vec3(1, 1, 1);
	}

	glm::mat4 getModelMatrix()
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, position);
		model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0, 0));
		model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1.0f, 0));
		model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1.0f));
		model = glm::scale(model, scale);

		return model;
	}

};