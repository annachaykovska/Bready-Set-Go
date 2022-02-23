#include "UISystem.h"

UISystem::UISystem() {
	//Freetype initialization
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		return;
	}

	FT_Face face;
	if (FT_New_Face(ft, "resources/fonts/OpenSans.ttf", 0, &face))
	{
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		return;
	}
	std::cout << "UI initialized" << std::endl;
}

UISystem::~UISystem(){}
