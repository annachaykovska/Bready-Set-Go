#pragma once

#include <glm/glm.hpp>

#include "../Scene/Component.h"

class Material : public Component
{
public:

	std::string name;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	glm::vec3 emissive;
	float shininess;
	float opacity;
	float refractionIndex;
};