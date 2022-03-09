#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glad/glad.h>
#include <glfw/glfw3.h>


class Shader {
public:

	Shader(std::string vertexPath, std::string fragmentPath);

	void checkCompileErrors(unsigned int ID, std::string name);
	void use();
	void cleanup();
	unsigned int getId();
	void setFloat(const std::string& name, unsigned int value);
	void setInt(const std::string& name, int value) const;
	void setMat4(const std::string& name, const glm::mat4& mat) const;
	void setVec3(const std::string& name, const glm::vec3& value) const;

private:

	unsigned int vertexShader;
	unsigned int fragmentShader;
	unsigned int shaderProgram;
};