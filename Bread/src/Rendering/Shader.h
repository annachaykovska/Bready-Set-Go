#pragma once

#include <string>



class Shader {
public:

	Shader(std::string vertexPath, std::string fragmentPath);

	void checkCompileErrors(unsigned int ID, std::string name);
	void use();
	void cleanup();
	unsigned int getId();
	void setFloat(const std::string& name, unsigned int value);

private:

	unsigned int vertexShader;
	unsigned int fragmentShader;
	unsigned int shaderProgram;
};