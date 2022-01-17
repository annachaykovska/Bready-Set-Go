#pragma once

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

class Texture2 {
public:

	Texture2(std::string path);
	Texture2();

	void setTexture(std::string path);
	std::string getPath() { return path;  }
	glm::ivec2 getDimensions() { return glm::ivec2(width, height); }
	void bind() { glBindTexture(GL_TEXTURE_2D, textureID); }
	void unbind() { glBindTexture(GL_TEXTURE_2D, textureID); }

private:

	std::string path;
	unsigned int textureID;
	int channels;
	int height;
	int width;
};