#include "Transform.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Scene/Entity.h"

Transform::Transform()
{
	this->position = glm::vec3(0, 0, 0);
	this->rotation = glm::vec3(0, 0, 0);
	this->rotationMat = glm::mat4(1.0f);
	this->worldRotationMat = glm::mat4(1.0f);
	this->scale = glm::vec3(1, 1, 1);
	this->heading = glm::vec3(1, 1, 1);
	this->model = glm::mat4(1.f);
}

void Transform::update()
{
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.f), this->position);
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.f), glm::radians(this->rotation.x), glm::vec3(1.f, 0, 0));
	rotationMatrix = glm::rotate(rotationMatrix, glm::radians(this->rotation.y), glm::vec3(0, 1.f, 0));
	rotationMatrix = glm::rotate(rotationMatrix, glm::radians(this->rotation.z), glm::vec3(0, 0, 1.f));
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.f), this->scale);

	this->model = translationMatrix * rotationMatrix * scaleMatrix;
}

void Transform::update(physx::PxTransform transform)
{
	this->position.x = transform.p.x;
	this->position.y = transform.p.y;
	this->position.z = transform.p.z;

	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.f), this->position);

	glm::quat newQuat = glm::quat(transform.q.w, transform.q.x, transform.q.y, transform.q.z);
	this->rotationMat = toMat4(newQuat);

	glm::quat worldQuat = glm::quat(transform.q.w, 0.f, transform.q.y, 0.f);
	this->worldRotationMat = toMat4(worldQuat);

	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.f), this->scale);

	this->heading = this->rotationMat * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f);

	this->model = translationMatrix * rotationMat * scaleMatrix;
}