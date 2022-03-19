#pragma once

#include "ImageTexture.h"
#include "Shader.h"
#include "../Transform.h"
#include "../SystemManager.h"
#include "../Scene/Scene.h"
#include "../Physics/PhysicsSystem.h"
#include <iostream>
#include <map>
#include <ft2build.h>
#include <glad/glad.h>
#include <glm/ext/vector_int2.hpp>
#include <glm/gtc/type_ptr.hpp>
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
	int checkForWin();
	void updateMiniMap(Transform& p1Transform, Transform& p2Transform, Transform& p3Transform, Transform& p4Transform);

private:


	void renderText(Shader& s, std::string text, float x, float y, float scale, glm::vec3 color);
	void renderImage(Shader& s, ImageTexture& image, float x, float y, float scaleX, float scaleY, float theta, float alpha);

	glm::vec3 offscreenBubbleLocation(glm::vec3 entityPos);
	glm::vec2 miniMapPlayerPosition(Transform& transform);

	float scX(float xVal);
	float scY(float yVal);
	float lerp(float p, float a, float b);

	ImageTexture speedometer;
	ImageTexture needle;
	ImageTexture miniMap;
	ImageTexture inventory;

	ImageTexture tomato;
	ImageTexture cheese;
	ImageTexture sausage;
	ImageTexture dough;

	ImageTexture cheeseOffscreen;

	ImageTexture pizza;

	ImageTexture p1Icon;
	ImageTexture p2Icon;
	ImageTexture p3Icon;
	ImageTexture p4Icon;

	glm::vec2 p1Location;
	glm::vec2 p2Location;
	glm::vec2 p3Location;
	glm::vec2 p4Location;

	//Constants needed to render
	std::map<char, Character> Characters;
	glm::mat4 projection;
	glm::mat4 imageProjection;
	Shader textShader;
	Shader imageShader;
	unsigned int VAO, VBO;
};