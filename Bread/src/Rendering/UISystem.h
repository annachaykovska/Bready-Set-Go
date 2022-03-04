#pragma once

#include "Shader.h"
#include <iostream>
#include <map>
#include <ft2build.h>
#include <glad/glad.h>
#include <glm/ext/vector_int2.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>

#include FT_FREETYPE_H

struct Character {
	unsigned int TextureID;  // ID handle of the glyph texture
	glm::ivec2   Size;       // Size of glyph
	glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
	unsigned int Advance;    // Offset to advance to next glyph
};

class UISystem
{
public:

	UISystem();
	~UISystem();

	void update();

private:
	void renderText(Shader& s, std::string text, float x, float y, float scale, glm::vec3 color);

	FT_Library ft;
	FT_Face face;
	std::map<char, Character> Characters;
	glm::mat4 projection;
	Shader textShader;
	unsigned int VAO, VBO;
};