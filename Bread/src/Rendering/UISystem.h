#pragma once

#include "ImageTexture.h"
#include "Shader.h"
#include "../Transform.h"
#include "../SystemManager.h"
#include "../Scene/Scene.h"
#include "../Physics/PhysicsSystem.h"
#include "../Navigation/IngredientTracker.h"
#include "../Audio/AudioSystem.h"
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

	//for tweaking
	float map_x;
	float map_sx;

	UISystem();
	~UISystem();
	
	void updatePlayer(unsigned int playerNum);
	void updateMainMenu(int itemSelected, int gameStage, int numCntrollers);
	void updateEndGame(int endScreenValue);
	void showPauseMenu(int itemSelected);
	void initIngredientTracking(IngredientTracker* tracker);
	int checkForWin();

private:

	void updateSpeedometer(int playerNum);
	void updateVacuum(int playerNum);
	void updateRecipeList();
	void updateMiniMap();
	void drawIndicator(Transform trans, ImageTexture& image);
	void updateOffscreenIndicators(int playerNum);
	void drawInventoryIcon(unsigned int haveItem, ImageTexture& image, unsigned int invPos);
	void updateInventory(int playerNum);

	void renderText(Shader& s, std::string text, float x, float y, float scale, glm::vec3 color);
	void renderImage(Shader& s, ImageTexture& image, float x, float y, float scaleX, float scaleY, float theta, float alpha);

	glm::vec3 offscreenBubbleLocation(glm::vec3 entityPos);
	glm::vec2 miniMapPlayerPosition(Transform& transform);

	float scX(float xVal);
	float scY(float yVal);
	float lerp(float p, float a, float b);                                                                  

	IngredientTracker* tracker;

	ImageTexture speedometer;
	float speedometer_theta;
	float p1SpeedometerTheta;
	float p2SpeedometerTheta;
	float p3SpeedometerTheta;
	float p4SpeedometerTheta;

	bool powerReady;
	ImageTexture needle;
	ImageTexture vacuum;
	ImageTexture vacuum_red;
	ImageTexture vacuum_yellow;
	ImageTexture vacuum_blue;
	ImageTexture vacuum_green;
	ImageTexture miniMap;
	ImageTexture inventory;

	ImageTexture tomato;
	ImageTexture cheese;
	ImageTexture sausage;
	ImageTexture dough;
	ImageTexture egg;
	ImageTexture peas;
	ImageTexture lettuce;
	ImageTexture chicken;
	ImageTexture rice;
	ImageTexture parsnip;
	ImageTexture carrot;

	ImageTexture cheeseOffscreen;
	ImageTexture tomatoOffscreen;
	ImageTexture doughOffscreen;
	ImageTexture sausageOffscreen;

	ImageTexture pizza;

	ImageTexture p1Icon;
	ImageTexture p2Icon;
	ImageTexture p3Icon;
	ImageTexture p4Icon;

	glm::vec2 p1Location;
	glm::vec2 p2Location;
	glm::vec2 p3Location;
	glm::vec2 p4Location;

	// Main Menu
	ImageTexture mainMenuBackground;
	ImageTexture startGameButtonNormal;
	ImageTexture startGameButtonPressed;
	ImageTexture exitButtonNormal;
	ImageTexture exitButtonPressed;

	ImageTexture singlePlayerButtonNormal;
	ImageTexture singlePlayerButtonPressed;
	ImageTexture multiPlayerButtonNormal;
	ImageTexture multiPlayerButtonPressed;

	ImageTexture playerConnected0;
	ImageTexture playerConnected1;
	ImageTexture playerConnected2;
	ImageTexture playerConnected3;
	ImageTexture playerConnected4;

	// Pause Menu
	ImageTexture pauseMenu;
	ImageTexture continueButton;
	ImageTexture continueButtonPressed;

	// End Game
	ImageTexture gameOverPlayer1_1;
	ImageTexture gameOverPlayer1_2;
	ImageTexture gameOverPlayer1_3;
	ImageTexture gameOverPlayer1_4;
	ImageTexture gameOverPlayer2;
	ImageTexture gameOverPlayer3;
	ImageTexture gameOverPlayer4;
	ImageTexture backToMainMenuButton;
	ImageTexture backToMainMenuButtonPressed;

	//Constants needed to render
	std::map<char, Character> Characters;
	glm::mat4 projection;
	glm::mat4 imageProjection;
	Shader textShader;
	Shader imageShader;
	unsigned int VAO, VBO;
};