#pragma once

class Shader {
public:

	Shader(std::string vertexPath, std::string fragmentPath);

	void checkCompileErrors(unsigned int ID, std::string name);
	void use() { glUseProgram(shaderProgram); }
	void cleanup() { glDeleteProgram(shaderProgram); }

private:

	unsigned int vertexShader;
	unsigned int fragmentShader;
	unsigned int shaderProgram;
};