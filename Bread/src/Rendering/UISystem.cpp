#include "UISystem.h"
#include "../Inventory.h"
#include "../Scene/Entity.h"
#include "../Gameplay/Recipe.h"
#include "../Gameplay/GameLoopManager.h"

extern SystemManager g_systems;
extern Scene g_scene;

namespace
{
    const float PI = 3.141592;
    const float MIN_SPEED_THETA = 3.f * PI / 4.f + 0.2f;
    const float MAX_SPEED_THETA = -PI / 4.f + 0.2f;
    const float MAX_NEEDLE_DELTA = (MIN_SPEED_THETA - MAX_SPEED_THETA) / 10.f;
}

UISystem::UISystem()
    : tracker(nullptr)
    , textShader("resources/shaders/textVertex.txt", "resources/shaders/textFragment.txt")
    , imageShader("resources/shaders/imageVertex.txt", "resources/shaders/imageFragment.txt")
    , speedometer("resources/textures/speedometer.png", GL_NEAREST)
    , needle("resources/textures/needle.png", GL_NEAREST)
    , vacuum("resources/textures/vacuum.png", GL_NEAREST)
    , vacuum_red("resources/textures/vacuum_red.png", GL_NEAREST)
    , vacuum_blue("resources/textures/vacuum_blue.png", GL_NEAREST)
    , vacuum_green("resources/textures/vacuum_green.png", GL_NEAREST)
    , vacuum_yellow("resources/textures/vacuum_yellow.png", GL_NEAREST)
    , miniMap("resources/textures/map.png", GL_NEAREST)
    , inventory("resources/textures/inventory.png", GL_NEAREST)
    , tomato("resources/textures/tomato.png", GL_NEAREST)
    , cheese("resources/textures/cheese.png", GL_NEAREST)
    , sausage("resources/textures/sausage.png", GL_NEAREST)
    , dough("resources/textures/dough.png", GL_NEAREST)
    , cheeseOffscreen("resources/textures/cheeseOffscreen.png", GL_NEAREST)
    , cheeseOffscreenUp("resources/textures/cheeseOffscreenUp.png", GL_NEAREST)
    , cheeseOffscreenDown("resources/textures/cheeseOffscreenDown.png", GL_NEAREST)
    , tomatoOffscreen("resources/textures/tomatoOffscreen.png", GL_NEAREST)
    , tomatoOffscreenUp("resources/textures/tomatoOffscreenUp.png", GL_NEAREST)
    , tomatoOffscreenDown("resources/textures/tomatoOffscreenDown.png", GL_NEAREST)
    , doughOffscreen("resources/textures/doughOffscreen.png", GL_NEAREST)
    , doughOffscreenUp("resources/textures/doughOffscreenUp.png", GL_NEAREST)
    , doughOffscreenDown("resources/textures/doughOffscreenDown.png", GL_NEAREST)
    , sausageOffscreen("resources/textures/sausageOffscreen.png", GL_NEAREST)
    , sausageOffscreenUp("resources/textures/sausageOffscreenUp.png", GL_NEAREST)
    , sausageOffscreenDown("resources/textures/sausageOffscreenDown.png", GL_NEAREST)
    , pizza("resources/textures/pizza.png", GL_NEAREST)
    , p1Icon("resources/textures/p1Icon.png", GL_NEAREST)
    , p2Icon("resources/textures/p2Icon.png", GL_NEAREST)
    , p3Icon("resources/textures/p3Icon.png", GL_NEAREST)
    , p4Icon("resources/textures/p4Icon.png", GL_NEAREST)
    , p1Location(glm::vec2(0))
    , p2Location(glm::vec2(0))
    , p3Location(glm::vec2(0))
    , p4Location(glm::vec2(0))
    , mainMenuBackground("resources/textures/main_menu_8.png", GL_NEAREST)
    , startGameButtonNormal("resources/textures/button_start_game_2.png", GL_NEAREST)
    , startGameButtonPressed("resources/textures/button_start_game_selected_2.png", GL_NEAREST)
    , exitButtonNormal("resources/textures/button_exit_2.png", GL_NEAREST)
    , exitButtonPressed("resources/textures/button_exit_selected_2.png", GL_NEAREST)
    , singlePlayerButtonNormal("resources/textures/button_single_player.png", GL_NEAREST)
    , singlePlayerButtonPressed("resources/textures/button_single_player_selected.png", GL_NEAREST)
    , multiPlayerButtonNormal("resources/textures/button_multi_player.png", GL_NEAREST)
    , multiPlayerButtonPressed("resources/textures/button_multi_player_selected.png", GL_NEAREST)
    , playerConnected0("resources/textures/player_connected_0.png", GL_NEAREST)
    , playerConnected1("resources/textures/player_connected_1.png", GL_NEAREST)
    , playerConnected2("resources/textures/player_connected_2.png", GL_NEAREST)
    , playerConnected3("resources/textures/player_connected_3.png", GL_NEAREST)
    , playerConnected4("resources/textures/player_connected_4.png", GL_NEAREST)
    , speedometer_theta(MIN_SPEED_THETA)
    , powerReady(true)
    , gameOverPlayer1_1("resources/textures/game_over_screen_player_1.png", GL_NEAREST)
    , gameOverPlayer1_2("resources/textures/game_over_screen_player_1_2.png", GL_NEAREST)
    , gameOverPlayer1_3("resources/textures/game_over_screen_player_1_3.png", GL_NEAREST)
    , gameOverPlayer1_4("resources/textures/game_over_screen_player_1_4.png", GL_NEAREST)
    , gameOverPlayer2("resources/textures/game_over_screen_player_2.png", GL_NEAREST)
    , gameOverPlayer3("resources/textures/game_over_screen_player_3.png", GL_NEAREST)
    , gameOverPlayer4("resources/textures/game_over_screen_player_4.png", GL_NEAREST)
    , backToMainMenuButton("resources/textures/button_back_to_main_menu.png", GL_NEAREST)
    , backToMainMenuButtonPressed("resources/textures/button_back_to_main_menu_selected.png", GL_NEAREST)
    , map_x(0.126)
    , map_sx(0.190)
    , pauseMenu("resources/textures/pause_screen.png", GL_NEAREST)
    , continueButton("resources/textures/button_continue.png", GL_NEAREST)
    , continueButtonPressed("resources/textures/button_continue_selected.png", GL_NEAREST) 
    , controlsMenu("resources/textures/controls_screen_3.png", GL_NEAREST)
    , semiTransparent("resources/textures/semi-transparent-block.png", GL_NEAREST)
{
    //Variables needed to initialize freetype characters
    FT_Library ft;
    FT_Face face;

    textShader.checkCompileErrors(textShader.getId(), "PROGRAM");
    projection = glm::ortho(0.f, float(g_systems.width), 0.f, float(g_systems.height)); //check if the max limits can be changed
    imageProjection = glm::ortho(0.f, 1.f, 0.f, 1.f);
	//Freetype initialization
	if (FT_Init_FreeType(&ft))
	{
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		return;
	}

    //Initializing font 
    //TODO: (perhaps this could be abstracted to use multiple fonts)
	if (FT_New_Face(ft, "resources/fonts/Night Action.ttf", 0, &face))
	{
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		return;
	}
	FT_Set_Pixel_Sizes(face, 0, 48); // Pixel font size

	//Initializing the character map
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
// set texture options
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
// now store character for later use
Character character = {
    texture,
    glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
    glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
    face->glyph->advance.x
};
//test print


Characters.insert(std::pair<char, Character>(c, character));
    }

    //Cleaning up the freetype variables
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // Enabling blending so that characters can have see through parts
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Binding the VAO and VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

UISystem::~UISystem() {

}

void UISystem::updateMainMenu(int itemSelected, int gameStage, int numControllers) {
    // Buttons
    if (gameStage == GameLoopMode::MENU_START) {
        if (itemSelected == 1) { // start game
            renderImage(imageShader, exitButtonNormal, scX(0.79f), scY(0.47f), scX(0.2f), scY(0.1f), 0, 1.f);
            renderImage(imageShader, startGameButtonPressed, scX(0.79f), scY(0.60f), scX(0.2f), scY(0.1f), 0, 1.f);
        }
        else if (itemSelected == 2) { // exit
            renderImage(imageShader, exitButtonPressed, scX(0.79f), scY(0.47f), scX(0.2f), scY(0.1f), 0, 1.f);
            renderImage(imageShader, startGameButtonNormal, scX(0.79f), scY(0.60f), scX(0.2f), scY(0.1f), 0, 1.f);
        }
    }
    else if (gameStage == GameLoopMode::MENU_SINGLE_MULTI_SELECTION) {
        if (itemSelected == 1) { // single player
            renderImage(imageShader, multiPlayerButtonNormal, scX(0.79f), scY(0.47f), scX(0.2f), scY(0.1f), 0, 1.f);
            renderImage(imageShader, singlePlayerButtonPressed, scX(0.79f), scY(0.60f), scX(0.2f), scY(0.1f), 0, 1.f);
        }
        else if (itemSelected == 2) { // multiplayer
            renderImage(imageShader, multiPlayerButtonPressed, scX(0.79f), scY(0.47f), scX(0.2f), scY(0.1f), 0, 1.f);
            renderImage(imageShader, singlePlayerButtonNormal, scX(0.79f), scY(0.60f), scX(0.2f), scY(0.1f), 0, 1.f);
        }

    }
    else if (gameStage == GameLoopMode::MENU_MULTI_CONNECT) {
        if (numControllers == 0) {
            renderImage(imageShader, playerConnected0, scX(0.79f), scY(0.60f), scX(0.3f), scY(0.2f), 0, 1.f);
        }
        if (numControllers == 1) {
            renderImage(imageShader, playerConnected1, scX(0.79f), scY(0.60f), scX(0.3f), scY(0.2f), 0, 1.f);
        }
        if (numControllers == 2) {
            renderImage(imageShader, playerConnected2, scX(0.79f), scY(0.60f), scX(0.3f), scY(0.2f), 0, 1.f);
        }
        if (numControllers == 3) {
            renderImage(imageShader, playerConnected3, scX(0.79f), scY(0.60f), scX(0.3f), scY(0.2f), 0, 1.f);
        }
        if (numControllers == 4) {
            renderImage(imageShader, playerConnected4, scX(0.79f), scY(0.60f), scX(0.3f), scY(0.2f), 0, 1.f);
        }
        renderImage(imageShader, startGameButtonPressed, scX(0.79f), scY(0.47f), scX(0.2f), scY(0.1f), 0, 1.f);
    }

    // Background
    renderImage(imageShader, mainMenuBackground, scX(0.5f), scY(0.5f), scX(1.0f), scY(1.0f), 0, 1.f);
}

void UISystem::updateEndGame(int endScreenValue) {
    int winner = checkForWin();
    std::string winText = "Player " + std::to_string(winner) + " Wins!";
    //renderText(textShader, winText, scX(0.4f), scY(0.48), 1.0f, glm::vec3(0.5, 0.5f, 0.5f));
    renderImage(imageShader, backToMainMenuButtonPressed, scX(0.5f), scY(0.4f), scX(0.2f), scY(0.1f), 0, 1.f);
    if (winner == 1) {
        if (endScreenValue == 0) {
            renderImage(imageShader, gameOverPlayer1_1, scX(0.5f), scY(0.53f), scX(0.4f), scY(0.7f), 0, 1.f);
        }
        else if (endScreenValue == 1) {
            renderImage(imageShader, gameOverPlayer1_2, scX(0.5f), scY(0.53f), scX(0.4f), scY(0.7f), 0, 1.f);
        }
        else if (endScreenValue == 2) {
            renderImage(imageShader, gameOverPlayer1_3, scX(0.5f), scY(0.53f), scX(0.4f), scY(0.7f), 0, 1.f);
        }
        else if (endScreenValue == 3) {
            renderImage(imageShader, gameOverPlayer1_4, scX(0.5f), scY(0.53f), scX(0.4f), scY(0.7f), 0, 1.f);
        }
    }
    else if (winner == 2)
        renderImage(imageShader, gameOverPlayer2, scX(0.5f), scY(0.53f), scX(0.4f), scY(0.7f), 0, 1.f);
    else if (winner == 3)
        renderImage(imageShader, gameOverPlayer3, scX(0.5f), scY(0.53f), scX(0.4f), scY(0.7f), 0, 1.f);
    else if (winner == 4)
        renderImage(imageShader, gameOverPlayer4, scX(0.5f), scY(0.53f), scX(0.4f), scY(0.7f), 0, 1.f);
}

void UISystem::showPauseMenu(int itemSelected) {
    if (itemSelected == 1) {
        renderImage(imageShader, continueButtonPressed, scX(0.73f), scY(0.52f), scX(0.2f), scY(0.1f), 0, 1.f);
        renderImage(imageShader, backToMainMenuButton, scX(0.73f), scY(0.39f), scX(0.2f), scY(0.1f), 0, 1.f);
    }
    else if (itemSelected == 2) {
        renderImage(imageShader, continueButton, scX(0.73f), scY(0.52f), scX(0.2f), scY(0.1f), 0, 1.f);
        renderImage(imageShader, backToMainMenuButtonPressed, scX(0.73f), scY(0.39f), scX(0.2f), scY(0.1f), 0, 1.f);
    }
    renderImage(imageShader, pauseMenu, scX(0.73f), scY(0.53f), scX(0.4f), scY(0.7f), 0, 1.f);
    renderImage(imageShader, controlsMenu, scX(0.23f), scY(0.53f), scX(0.8f), scY(0.8f), 0, 1.f);
    renderImage(imageShader, semiTransparent, scX(0.5f), scY(0.5f), scX(1.0f), scY(1.0f), 0, 1.f);
}


void UISystem::updateGame(int endScreenValue, int pauseMenuItemSelected, bool pause) {
    if (pause) {
        showPauseMenu(pauseMenuItemSelected);
    }

    if (checkForWin() != 0)
    {
        updateEndGame(endScreenValue);
    }
    float height, width;

    // Player 1 UI (eventually abstract to a draw player UI method)
    // Drawing speedometer
    float speedometer_goal_theta = lerp(std::min(std::max(abs(g_systems.physics->getPlayerSpeed(1)) / 40.f, 0.f), 1.f), MIN_SPEED_THETA, MAX_SPEED_THETA);
    speedometer_theta += std::min(std::max((speedometer_goal_theta - speedometer_theta), -MAX_NEEDLE_DELTA), MAX_NEEDLE_DELTA);
    renderImage(imageShader, needle, scX(0.875), scY(0.19), scX(0.225), scY(0.3), speedometer_theta, 1.f);

    auto p1_ent = g_scene.getEntity("player1");
    if ((glfwGetTime() - p1_ent->lastMagnetUse) / p1_ent->magnetCooldown > 0.99f && powerReady == false)
    {
        g_systems.audio->powerReady(p1_ent->getAudioSource());
        powerReady = true;
    }
    else if ((glfwGetTime() - p1_ent->lastMagnetUse) / p1_ent->magnetCooldown < 0.99f && powerReady == true)
    {
        powerReady = false;
    }
    // TODO: Needs to be refactored when multiplayer is implemented
    ImageTexture* vac_status;
    double vac_alpha = 1.0;
    if (p1_ent->magnetStatus == 0) {
        vac_status = &vacuum_red;
        vac_alpha = ((glfwGetTime() - p1_ent->lastMagnetUse) / p1_ent->magnetCooldown) * 0.75;
    } else if (p1_ent->magnetStatus == 1) vac_status = &vacuum_yellow;
    else if (p1_ent->magnetStatus == 2) vac_status = &vacuum_green;
    else vac_status = &vacuum_blue;
    
    renderImage(imageShader, *vac_status, scX(0.875), scY(0.2), scX(0.1), scY(0.1), 0.f, vac_alpha);

    renderImage(imageShader, speedometer, scX(0.875), scY(0.19), scX(0.225), scY(0.3), 0.f, 1.f);

    

    Recipe* p1Recipe = (Recipe*)g_scene.getEntity("player1")->getComponent("recipe");
    Recipe* p2Recipe = (Recipe*)g_scene.getEntity("player2")->getComponent("recipe");
    Recipe* p3Recipe = (Recipe*)g_scene.getEntity("player3")->getComponent("recipe");
    Recipe* p4Recipe = (Recipe*)g_scene.getEntity("player4")->getComponent("recipe");

    if (p1Recipe != nullptr)
    {
        // Recipe States
        renderText(textShader, "Recipe Progress", scX(0.8), scY(0.9), 1.f, glm::vec3(1, 1, 1));
        renderText(textShader, "P1 ", scX(0.8), scY(0.85), 1.f, glm::vec3(1, 0, 0));
        renderText(textShader, "P2 ", scX(0.8), scY(0.8), 1.f, glm::vec3(0, 0, 1));
        renderText(textShader, "P3 ", scX(0.8), scY(0.75), 1.f, glm::vec3(0, 1, 0));
        renderText(textShader, "P4" , scX(0.8), scY(0.7), 1.f, glm::vec3(1, 1, 0));
        renderText(textShader, "(Pizza): " + std::to_string(p1Recipe->progress) + "/4", scX(0.83), scY(0.85), 1.f, glm::vec3(1, 1, 1));
        renderText(textShader, "(Omelette): " + std::to_string(p2Recipe->progress) + "/4", scX(0.83), scY(0.8), 1.f, glm::vec3(1, 1, 1));
        renderText(textShader, "(Wrap): " + std::to_string(p3Recipe->progress) + "/4", scX(0.83), scY(0.75), 1.f, glm::vec3(1, 1, 1));
        renderText(textShader, "(Salad): " + std::to_string(p4Recipe->progress) + "/4", scX(0.83), scY(0.7), 1.f, glm::vec3(1, 1, 1));
    }

    // Drawing minimap
    renderImage(imageShader, p1Icon, p1Location.x, p1Location.y, 20.f, 20.f, 0, 1.f);
    renderImage(imageShader, p2Icon, p2Location.x, p2Location.y, 20.f, 20.f, 0, 1.f);
    renderImage(imageShader, p3Icon, p3Location.x, p3Location.y, 20.f, 20.f, 0, 1.f);
    renderImage(imageShader, p4Icon, p4Location.x, p4Location.y, 20.f, 20.f, 0, 1.f);

    renderImage(imageShader, miniMap, scX(map_x), scY(0.8), scX(map_sx), scY(0.325), 0, 1.f);

    glm::vec3 IngLocation;

    Entity* player1 = g_scene.getEntity("player1");
    Inventory* p1Inv = (Inventory*)player1->getComponent("inventory");

    if (!p1Inv->cheese)
    {
        int cheeseY;
        IngLocation = offscreenBubbleLocation(tracker->getCheeseLocation().position, cheeseY);
        if (cheeseY == 0)
        {
            renderImage(imageShader, cheeseOffscreen, IngLocation.x, IngLocation.y, IngLocation.z, IngLocation.z, 0, 1.f);
        }
        else if (cheeseY == 1)
        {
            renderImage(imageShader, cheeseOffscreenUp, IngLocation.x, IngLocation.y, IngLocation.z, IngLocation.z, 0, 1.f);
        }
        else
        {
            renderImage(imageShader, cheeseOffscreenDown, IngLocation.x, IngLocation.y, IngLocation.z, IngLocation.z, 0, 1.f);
        }
    }

    if (!p1Inv->tomato)
    {
        int tomatoY;
        IngLocation = offscreenBubbleLocation(tracker->getTomatoLocation().position, tomatoY);
        if (tomatoY == 0)
        {
            renderImage(imageShader, tomatoOffscreen, IngLocation.x, IngLocation.y, IngLocation.z, IngLocation.z, 0, 1.f);
        }
        else if (tomatoY == 1)
        {
            renderImage(imageShader, tomatoOffscreenUp, IngLocation.x, IngLocation.y, IngLocation.z, IngLocation.z, 0, 1.f);
        }
        else
        {
            renderImage(imageShader, tomatoOffscreenDown, IngLocation.x, IngLocation.y, IngLocation.z, IngLocation.z, 0, 1.f);
        }
    }

    if (!p1Inv->dough)
    {
        int doughY;
        IngLocation = offscreenBubbleLocation(tracker->getDoughLocation().position, doughY);
        if (doughY == 0)
        {
            renderImage(imageShader, doughOffscreen, IngLocation.x, IngLocation.y, IngLocation.z, IngLocation.z, 0, 1.f);
        }
        else if (doughY == 1)
        {
            renderImage(imageShader, doughOffscreenUp, IngLocation.x, IngLocation.y, IngLocation.z, IngLocation.z, 0, 1.f);
        }
        else
        {
            renderImage(imageShader, doughOffscreenDown, IngLocation.x, IngLocation.y, IngLocation.z, IngLocation.z, 0, 1.f);
        }
    }

    if (!p1Inv->sausage)
    {
        int sausageY;
        IngLocation = offscreenBubbleLocation(tracker->getSausageLocation().position, sausageY);
        if (sausageY == 0)
        {
            renderImage(imageShader, sausageOffscreen, IngLocation.x, IngLocation.y, IngLocation.z, IngLocation.z, 0, 1.f);
        }
        else if (sausageY == 1)
        {
            renderImage(imageShader, sausageOffscreenUp, IngLocation.x, IngLocation.y, IngLocation.z, IngLocation.z, 0, 1.f);
        }
        else
        {
            renderImage(imageShader, sausageOffscreenDown, IngLocation.x, IngLocation.y, IngLocation.z, IngLocation.z, 0, 1.f);
        }
    }

    // Drawing Inventory
    float alpha;
    float faded = 0.15f;
    float opaque = 1.f;

    float invScaleX = scX(0.04);
    float invScaleY = scY(0.06);
    float invXOffset = scX(0.12);
    float invYOffset = scY(0.4);
    float invBlockOffset = scY(0.097);
    float recipeYOffset = scY(0.53);

    alpha = (p1Inv->tomato) ? opaque : faded;
    renderImage(imageShader, tomato, invXOffset, invYOffset - (0 * invBlockOffset), invScaleX, invScaleY, 0, alpha);

    alpha = (p1Inv->cheese) ? opaque : faded;
    renderImage(imageShader, cheese, invXOffset, invYOffset - (1 * invBlockOffset), invScaleX, invScaleY, 0, alpha);
    
    alpha = (p1Inv->dough) ? opaque : faded;
    renderImage(imageShader, dough, invXOffset, invYOffset - (2 * invBlockOffset), invScaleX, invScaleY, 0, alpha);

    alpha = (p1Inv->sausage) ? opaque : faded;
    renderImage(imageShader, sausage, invXOffset, invYOffset - (3 * invBlockOffset), invScaleX, invScaleY, 0, alpha);

    alpha = (p1Inv->tomato && p1Inv->cheese && p1Inv->dough && p1Inv->sausage) ? opaque : faded;
    renderImage(imageShader, pizza, invXOffset, recipeYOffset, scX(0.06), scY(0.09), 0, alpha);

    renderImage(imageShader, inventory, invXOffset, scY(0.33), scX(0.1), scY(0.53), 0, 1.f);

}

void UISystem::initIngredientTracking(IngredientTracker* offscreenTracker)
{
    tracker = offscreenTracker;
}

int UISystem::checkForWin()
{
    Entity* player1 = g_scene.getEntity("player1");
    Inventory* p1Inv = (Inventory*)player1->getComponent("inventory");

    if (p1Inv->tomato && p1Inv->cheese && p1Inv->dough && p1Inv->sausage)
    {
        return 1;
    }

    Entity* player2 = g_scene.getEntity("player2");
    Inventory* p2Inv = (Inventory*)player2->getComponent("inventory");

    if (p2Inv->egg && p2Inv->cheese && p2Inv->peas && p2Inv->lettuce)
    {
        return 2;
    }

    Entity* player3 = g_scene.getEntity("player3");
    Inventory* p3Inv = (Inventory*)player3->getComponent("inventory");

    if (p3Inv->chicken && p3Inv->dough && p3Inv->rice && p3Inv->lettuce)
    {
        return 3;
    }

    Entity* player4 = g_scene.getEntity("player4");
    Inventory* p4Inv = (Inventory*)player4->getComponent("inventory");

    if (p4Inv->parsnip && p4Inv->carrot && p4Inv->tomato && p4Inv->lettuce)
    {
        return 4;
    }

    return 0;
}

void UISystem::updateMiniMap(Transform& p1Transform, Transform& p2Transform, Transform& p3Transform, Transform& p4Transform)
{
    p1Location = miniMapPlayerPosition(p1Transform);
    p2Location = miniMapPlayerPosition(p2Transform);
    p3Location = miniMapPlayerPosition(p3Transform);
    p4Location = miniMapPlayerPosition(p4Transform);
}

glm::vec2 UISystem::miniMapPlayerPosition(Transform& transform)
{
    float x;
    float z;
    glm::vec2 location;
    //x = 0 + ((400.f - 0) / (270.f - (-270.f))) * (transform.position.x - (-270.f));
    //z = 0 + ((400.f - 0) / (210.f - (-170.f))) * (transform.position.z - (-170.f));
    x = 0 + ((scX(0.15625) - 0) / (270.f - (-270.f))) * (transform.position.x - (-270.f));
    z = 0 + ((scY(0.28) - 0) / (210.f - (-170.f))) * (transform.position.z - (-170.f));
    location = glm::vec2(x, z);
    location.y = scY(0.3) - location.y;
    location += glm::vec2(scX(0.04), scY(0.62));
    return location;
}

void UISystem::renderText(Shader& s, std::string text, float x, float y, float scale, glm::vec3 color) 
{
    // activate corresponding render state	
    s.use();
    glUniformMatrix4fv(glGetUniformLocation(s.getId(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3f(glGetUniformLocation(s.getId(), "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;



        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };

        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void UISystem::renderImage(Shader& s, ImageTexture& image, float x, float y, float scaleX, float scaleY, float theta, float alpha)
{
    s.use();
    glUniformMatrix4fv(glGetUniformLocation(s.getId(), "projection"), 1, GL_FALSE, glm::value_ptr(imageProjection));
    glUniform1f(glGetUniformLocation(s.getId(), "alpha"), alpha);    
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // THIS SHOULD PROBABLY BE CHANGED EVENTUALLY
    float vertices[6][4] = {
        {-1., 1., 0., 1.},
        {-1., -1., 0., 0.},
        {1., -1., 1., 0.},

        {-1., 1., 0., 1.},
        {1., -1., 1., 0.},
        {1., 1., 1., 1.}
    };
    glm::mat4 rotate = glm::rotate(glm::mat4(1.f), theta, glm::vec3(0.f,0.f,1.f));
    glm::mat4 scale = glm::scale(glm::mat4(1.f), glm::vec3(scaleX/g_systems.width, scaleY/g_systems.height, 1.f));
    glm::mat4 translation = glm::translate(glm::mat4(1.f), glm::vec3((x - (g_systems.width / 2.f)) / (g_systems.width / 2.f), (y - (g_systems.height / 2.f)) / (g_systems.height / 2.f), 0.f));
    glm::mat4 modelMat = translation * scale * rotate;

    glUniformMatrix4fv(glGetUniformLocation(s.getId(), "model"), 1, GL_FALSE, glm::value_ptr(modelMat));

    // render glyph texture over quad
    image.bind();
    // update content of VBO memory
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // render quad
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

glm::vec3 UISystem::offscreenBubbleLocation(glm::vec3 entityPos, int& vertical)
{
    vertical = 0;
    bool verticalOffscreen = false;
    
    glm::vec4 location = glm::vec4(entityPos, 1);

    glm::vec3 cam = g_scene.camera.centerBeam;
    glm::vec3 toEntity = entityPos - g_scene.camera.position;

    glm::mat4 viewMatrix = g_scene.camera.getViewMatrix(g_scene.getEntity("player1")->getTransform());
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(g_scene.camera.getPerspective()), float(g_systems.width) / g_systems.height, 0.1f, 500.0f);

    location = projectionMatrix * viewMatrix * location;

    location.x /= location.w;
    location.y /= location.w;
    location.z /= location.w;

    location.x = (location.x + 1) * g_systems.width * 0.5;
    location.y = (location.y + 1) * g_systems.height * 0.5 + scY(0.1);
    
    if(location.z < -1 || location.z > 1 || location.x > g_systems.width || location.x < 0)
    {
        vertical = 0;
        // When location.z is not in the interval between [-1, 1] it is no longer in the camera frustrum.
        // This where you will need to put your code for the object offscreen beside and behind states. 

        glm::vec3 yaw = glm::vec3(toEntity.x, 0, toEntity.z);

        float viewRange = g_scene.camera.perspective / 2.f;
        //float viewRange = 80 / 2.f;
        viewRange *= (PI / 180.f);

        float yawTheta = acos(dot(cam, yaw) / (length(yaw) * length(cam)));
        glm::vec3 upVector = cross(cam, yaw);

        float pitchTheta = acos(dot(toEntity, yaw) / (length(yaw) * length(toEntity)));
        
        // If the entity is behind the player
        if (yawTheta > (PI / 2.f))
        {
            location.y = scY(0.07);

            float offset = (((g_systems.width / 2.f) / (PI - (PI / 2.f))) * (yawTheta - (PI / 2.f)));
            if (upVector.y < 0)
            {
                location.x = g_systems.width - scX(0.02) - offset;
            }
            else
            {
                location.x = scX(0.02) + offset;
            }
        }
        // If the entity is between 90 deg and the viewrange of the player
        else
        {
            float offset = (((((g_systems.height - scY(0.0375)) / 2.f) - scY(0.0375)) / ((PI / 2.f) - viewRange)) * (yawTheta - viewRange));
            if (upVector.y < 0)
            {
                location.x = g_systems.width - scX(0.0375);
                location.y = (g_systems.height / 2.f) - offset;
            }
            else
            {
                location.x = scX(0.0375);
                location.y = (g_systems.height / 2.f) - offset;
            }
        }
        location.z = scX(0.04);
    }
    else if (location.y > g_systems.height)
    {
        vertical = 1;
        location.y = g_systems.height - scX(0.0375);
        location.z = scX(0.04);
        verticalOffscreen = true;
    }
    else if (location.y < 0)
    {
        vertical = -1;
        location.y = scY(0.0375);
        location.z = scX(0.04);
        verticalOffscreen = true;
    }
    else
    {
        location.z = scX(0.07) + ((scX(0.04) - scX(0.07)) / (400.f - 0.f)) * (length(toEntity) - 0);
    }

    if (location.z < scX(0.04))
    {
        location.z = scX(0.04);
    }

    return glm::vec3(location.x, location.y, location.z);
}

float UISystem::scX(float xVal)
{
    return xVal * g_systems.width;
}

float UISystem::scY(float yVal)
{
    return yVal * g_systems.height;
}

float UISystem::lerp(float p, float a, float b) {
    return (1 - p) * a + p * b;
}
