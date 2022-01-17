#pragma once

#include <glad/glad.h>

class Shader {
public:

	Shader(std::string vertexPath, std::string fragmentPath);

	void checkCompileErrors(unsigned int ID, std::string name);
	void use() { glUseProgram(shaderProgram); }
	void cleanup() { glDeleteProgram(shaderProgram); }
	unsigned int getId() { return shaderProgram; }
	void setFloat(const std::string& name, unsigned int value)
	{
		glUniform1f(glGetUniformLocation(shaderProgram, name.c_str()), value);
	}

private:

	unsigned int vertexShader;
	unsigned int fragmentShader;
	unsigned int shaderProgram;
};