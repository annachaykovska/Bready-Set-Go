#pragma once

#include <string>

#include <glm/glm.hpp>

struct Component {
	unsigned int entityID;
};

struct Transform : Component {
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};