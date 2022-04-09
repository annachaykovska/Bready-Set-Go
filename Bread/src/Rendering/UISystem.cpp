#include "UISystem.h"
#include "../Inventory.h"
#include "../Scene/Entity.h"
#include "../Gameplay/Recipe.h"
#include "../Gameplay/GameLoopManager.h"
#include "RenderingSystem.h"

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
    , egg("resources/textures/egg.png", GL_NEAREST)
    , peas("resources/textures/peas.png", GL_NEAREST)
    , lettuce("resources/textures/lettuce.png", GL_NEAREST)
    , chicken("resources/textures/chicken.png", GL_NEAREST)
    , rice("resources/textures/rice.png", GL_NEAREST)
    , parsnip("resources/textures/parsnip.png", GL_NEAREST)
    , carrot("resources/textures/carrot.png", GL_NEAREST)
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
    , unflip3("resources/textures/unflip3.png", GL_NEAREST)
    , unflip2("resources/textures/unflip2.png", GL_NEAREST)
    , unflip1("resources/textures/unflip1.png", GL_NEAREST)
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
    , p1SpeedometerTheta(MIN_SPEED_THETA)
    , p2SpeedometerTheta(MIN_SPEED_THETA)
    , p3SpeedometerTheta(MIN_SPEED_THETA)
    , p4SpeedometerTheta(MIN_SPEED_THETA)
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

// Draws the speedometer for the passed player HUD
void UISystem::updateSpeedometer(int playerNum)
{
    // Drawing speedometer
    float speedometer_goal_theta = lerp(std::min(std::max(abs(g_systems.physics->getPlayerSpeed(playerNum)) / 40.f, 0.f), 1.f), MIN_SPEED_THETA, MAX_SPEED_THETA);
    float speedometer_theta = 0.0f;

    switch (playerNum)
    {
    case 1:
        p1SpeedometerTheta += std::min(std::max((speedometer_goal_theta - p1SpeedometerTheta), -MAX_NEEDLE_DELTA), MAX_NEEDLE_DELTA);
        speedometer_theta = p1SpeedometerTheta;
        break;
    case 2:
        p2SpeedometerTheta += std::min(std::max((speedometer_goal_theta - p2SpeedometerTheta), -MAX_NEEDLE_DELTA), MAX_NEEDLE_DELTA);
        speedometer_theta = p2SpeedometerTheta;
        break;
    case 3:
        p3SpeedometerTheta += std::min(std::max((speedometer_goal_theta - p3SpeedometerTheta), -MAX_NEEDLE_DELTA), MAX_NEEDLE_DELTA);
        speedometer_theta = p3SpeedometerTheta;
        break;
    case 4:
        p4SpeedometerTheta += std::min(std::max((speedometer_goal_theta - p4SpeedometerTheta), -MAX_NEEDLE_DELTA), MAX_NEEDLE_DELTA);
        speedometer_theta = p4SpeedometerTheta;
        break;
    default:
        return; // Uh-oh, something has gone wrong
        break;
    }

    //speedometer_theta += std::min(std::max((speedometer_goal_theta - speedometer_theta), -MAX_NEEDLE_DELTA), MAX_NEEDLE_DELTA);

    renderImage(imageShader, needle, scX(0.875), scY(0.19), scX(0.225), scY(0.3), speedometer_theta, 1.f);
    renderImage(imageShader, speedometer, scX(0.875), scY(0.19), scX(0.225), scY(0.3), 0.f, 1.f);
}

// Draws the vacuum item for the passed player HUD
void UISystem::updateVacuum(int playerNum)
{
    Entity* player = nullptr;

    // Get reference to correct player Entity
    switch (playerNum)
    {
    case 1:
        player = g_scene.getEntity("player1");
        break;
    case 2:
        player = g_scene.getEntity("player2");
        break;
    case 3:
        player = g_scene.getEntity("player3");
        break;
    case 4:
        player = g_scene.getEntity("player4");
        break;
    default:
        return; // Uh-oh, something has gone wrong
        break;
    }

    // Play sound effect if vacuum power has reset
    if ((glfwGetTime() - player->lastMagnetUse) / player->magnetCooldown > 0.99f && powerReady == false)
    {
        g_systems.audio->powerReady(player->getAudioSource());
        powerReady = true;
    }
    else if ((glfwGetTime() - player->lastMagnetUse) / player->magnetCooldown < 0.99f && powerReady == true)
    {
        powerReady = false;
    }

    // Update vacuum icon on HUD
    ImageTexture* vac_status;
    double vac_alpha = 1.0;
    
    if (player->magnetStatus == 1)      // Charged, out of range
    {
        vac_status = &vacuum_yellow;
    }
    else if (player->magnetStatus == 0) // Uncharged
    {
        vac_status = &vacuum_red;
        vac_alpha = ((glfwGetTime() - player->lastMagnetUse) / player->magnetCooldown) * 0.75;
    }
    else if (player->magnetStatus == 2) // Charged, in range
    {
        vac_status = &vacuum_green;
    }
    else                                // Actively stealing
    {
        vac_status = &vacuum_blue;
    }

    renderImage(imageShader, *vac_status, scX(0.875), scY(0.2), scX(0.1), scY(0.1), 0.f, vac_alpha);
}

// Draws the recipe state for each player to the UI
void UISystem::updateRecipeList()
{
    Recipe* p1Recipe = (Recipe*)g_scene.getEntity("player1")->getComponent("recipe");
    Recipe* p2Recipe = (Recipe*)g_scene.getEntity("player2")->getComponent("recipe");
    Recipe* p3Recipe = (Recipe*)g_scene.getEntity("player3")->getComponent("recipe");
    Recipe* p4Recipe = (Recipe*)g_scene.getEntity("player4")->getComponent("recipe");

    // Recipe States
    renderText(textShader, "Recipe Progress", scX(0.8), scY(0.9), 1.f, glm::vec3(1, 1, 1));
    renderText(textShader, "P1 ", scX(0.8), scY(0.85), 1.f, glm::vec3(1, 0, 0));
    renderText(textShader, "P2 ", scX(0.8), scY(0.8), 1.f, glm::vec3(0, 0, 1));
    renderText(textShader, "P3 ", scX(0.8), scY(0.75), 1.f, glm::vec3(0, 1, 0));
    renderText(textShader, "P4", scX(0.8), scY(0.7), 1.f, glm::vec3(1, 1, 0));
    renderText(textShader, "(Pizza): " + std::to_string(p1Recipe->progress) + "/4", scX(0.83), scY(0.85), 1.f, glm::vec3(1, 1, 1));
    renderText(textShader, "(Omelette): " + std::to_string(p2Recipe->progress) + "/4", scX(0.83), scY(0.8), 1.f, glm::vec3(1, 1, 1));
    renderText(textShader, "(Wrap): " + std::to_string(p3Recipe->progress) + "/4", scX(0.83), scY(0.75), 1.f, glm::vec3(1, 1, 1));
    renderText(textShader, "(Salad): " + std::to_string(p4Recipe->progress) + "/4", scX(0.83), scY(0.7), 1.f, glm::vec3(1, 1, 1));
}

// Helper function that renders the passed ImageTexture at the passed Transform
void UISystem::drawIndicator(int playerNum, Transform trans, ImageTexture& image, ImageTexture& imageUp, ImageTexture& imageDown)
{
    int indicatorY;

    glm::vec3 IngLocation = offscreenBubbleLocation(playerNum, trans.position, indicatorY);

    if (indicatorY == 0)
        renderImage(imageShader, image, IngLocation.x, IngLocation.y, IngLocation.z, IngLocation.z, 0, 1.f);
    else if (indicatorY == 1)
        renderImage(imageShader, imageUp, IngLocation.x, IngLocation.y, IngLocation.z, IngLocation.z, 0, 1.f);
    else
        renderImage(imageShader, imageDown, IngLocation.x, IngLocation.y, IngLocation.z, IngLocation.z, 0, 1.f);
}

// TODO need to update image references for players 2-3 when we have the new icons
// Draws the offscreen indicators for missing ingredients to the UI
void UISystem::updateOffscreenIndicators(int playerNum)
{
    Inventory* inventory = nullptr;

    switch (playerNum)
    {
    case 1: // cheese, tomato, dough, sausage

        inventory = g_scene.getEntity("player1")->getInventory();

        if (!inventory->cheese)
            drawIndicator(playerNum, tracker->getCheeseLocation(), cheeseOffscreen, cheeseOffscreenUp, cheeseOffscreenDown);
        if (!inventory->tomato)
            drawIndicator(playerNum, tracker->getTomatoLocation(), tomatoOffscreen, tomatoOffscreenUp, tomatoOffscreenDown);
        if (!inventory->dough)
            drawIndicator(playerNum, tracker->getDoughLocation(), doughOffscreen, doughOffscreenUp, doughOffscreenDown);
        if (!inventory->sausage)
            drawIndicator(playerNum, tracker->getSausageLocation(), sausageOffscreen, sausageOffscreenUp, sausageOffscreenDown);
        break;

    case 2: // egg, cheese, peas, lettuce

        inventory = g_scene.getEntity("player2")->getInventory();

        if (!inventory->egg)
            drawIndicator(playerNum, tracker->getEggLocation(), egg, egg, egg);
        if (!inventory->cheese)
            drawIndicator(playerNum, tracker->getCheeseLocation(), cheeseOffscreen, cheeseOffscreenUp, cheeseOffscreenDown); 
        if (!inventory->peas)
            drawIndicator(playerNum, tracker->getPeasLocation(), peas, peas, peas);
        if (!inventory->lettuce)
            drawIndicator(playerNum, tracker->getLettuceLocation(), lettuce, lettuce, lettuce);
        break;

    case 3: // chicken, dough, rice, lettuce

        inventory = g_scene.getEntity("player3")->getInventory();

        if (!inventory->chicken)
            drawIndicator(playerNum, tracker->getChickenLocation(), chicken, chicken, chicken);
        if (!inventory->dough)
            drawIndicator(playerNum, tracker->getDoughLocation(), doughOffscreen, doughOffscreenUp, doughOffscreenDown);
        if (!inventory->rice)
            drawIndicator(playerNum, tracker->getRiceLocation(), rice, rice, rice);
        if (!inventory->lettuce)
            drawIndicator(playerNum, tracker->getLettuceLocation(), lettuce, lettuce, lettuce);
        break;

    case 4: // parsnip, carrot, tomato, lettuce

        inventory = g_scene.getEntity("player4")->getInventory();

        if (!inventory->parsnip)
            drawIndicator(playerNum, tracker->getParsnipLocation(), parsnip, parsnip, parsnip);
        if (!inventory->carrot)
            drawIndicator(playerNum, tracker->getCarrotLocation(), carrot, carrot, carrot);
        if (!inventory->tomato)
            drawIndicator(playerNum, tracker->getTomatoLocation(), tomatoOffscreen, tomatoOffscreenUp, tomatoOffscreenDown);
        if (!inventory->lettuce)
            drawIndicator(playerNum, tracker->getLettuceLocation(), lettuce, lettuce, lettuce);
        break;

    default:
        return; // Uh-oh, something has gone wrong
        break;
    }
}

void UISystem::drawInventoryIcon(unsigned int haveItem, ImageTexture& image, unsigned int invPos)
{
    float alpha;
    float faded = 0.15f;
    float opaque = 1.f;

    float invScaleX = scX(0.04);
    float invScaleY = scY(0.06);
    float invXOffset = scX(0.12);
    float invYOffset = scY(0.4);
    float invBlockOffset = scY(0.097);
    float recipeYOffset = scY(0.53);

    alpha = (haveItem) ? opaque : faded;
    renderImage(imageShader, image, invXOffset, invYOffset - (invPos * invBlockOffset), invScaleX, invScaleY, 0, alpha);
}

// TODO need to update image references for players 2-3 when we have the new icons
// Draws the player's inventory to the HUD
void UISystem::updateInventory(int playerNum)
{
    Inventory* playerInv = nullptr;

    float alpha;
    float faded = 0.15f;
    float opaque = 1.f;

    float invXOffset = scX(0.12);
    float recipeYOffset = scY(0.53);

    // Draw inventory icons for this player
    switch (playerNum)
    {
    case 1: // tomato, cheese, dough, sausage, pizza

        playerInv = g_scene.getEntity("player1")->getInventory();

        drawInventoryIcon(playerInv->tomato, tomato, 0);
        drawInventoryIcon(playerInv->cheese, cheese, 1);
        drawInventoryIcon(playerInv->dough, dough, 2);
        drawInventoryIcon(playerInv->sausage, sausage, 3);

        alpha = (playerInv->tomato && playerInv->cheese && playerInv->dough && playerInv->sausage) ? opaque : faded;
        renderImage(imageShader, pizza, invXOffset, recipeYOffset, scX(0.06), scY(0.09), 0, alpha);

        break;

    case 2: // egg, cheese, peas, lettuce, ????

        playerInv = g_scene.getEntity("player2")->getInventory();

        drawInventoryIcon(playerInv->egg, egg, 0);
        drawInventoryIcon(playerInv->cheese, cheese, 1);
        drawInventoryIcon(playerInv->peas, peas, 2);
        drawInventoryIcon(playerInv->lettuce, lettuce, 3);

        alpha = (playerInv->egg && playerInv->cheese && playerInv->peas && playerInv->lettuce) ? opaque : faded;
        renderImage(imageShader, cheese, invXOffset, recipeYOffset, scX(0.06), scY(0.09), 0, alpha);

        break;

    case 3: // chicken, dough, rice, lettuce, ????

        playerInv = g_scene.getEntity("player3")->getInventory();

        drawInventoryIcon(playerInv->chicken, chicken, 0);
        drawInventoryIcon(playerInv->dough, dough, 1);
        drawInventoryIcon(playerInv->rice, rice, 2);
        drawInventoryIcon(playerInv->lettuce, lettuce, 3);

        alpha = (playerInv->chicken && playerInv->dough && playerInv->rice && playerInv->lettuce) ? opaque : faded;
        renderImage(imageShader, dough, invXOffset, recipeYOffset, scX(0.06), scY(0.09), 0, alpha);

        break;

    case 4: // parsnip, carrot, tomato, lettuce, salad

        playerInv = g_scene.getEntity("player4")->getInventory();

        drawInventoryIcon(playerInv->parsnip, parsnip, 0);
        drawInventoryIcon(playerInv->carrot, carrot, 1);
        drawInventoryIcon(playerInv->tomato, tomato, 2);
        drawInventoryIcon(playerInv->lettuce, lettuce, 3);

        alpha = (playerInv->parsnip && playerInv->carrot && playerInv->tomato && playerInv->lettuce) ? opaque : faded;
        renderImage(imageShader, tomato, invXOffset, recipeYOffset, scX(0.06), scY(0.09), 0, alpha);

        break;

    default:
        return; // Uh-oh, something has gone wrong
        break;
    }

    // Draw inventory frame
    renderImage(imageShader, this->inventory, invXOffset, scY(0.33), scX(0.1), scY(0.53), 0, 1.f);
}

void UISystem::updatePlayer(unsigned int playerNum)
{
    if (checkForWin() != 0)
        updateEndGame(g_systems.loop->endScreenGenerated);
    
    updateVacuum(playerNum);
    updateSpeedometer(playerNum);
    updateRecipeList();
    updateMiniMap();
    updateOffscreenIndicators(playerNum);
    updateInventory(playerNum);
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

void UISystem::updateMiniMap()
{
    // Get new locations for minimap icons
    this->p1Location = miniMapPlayerPosition(*g_scene.getEntity("player1")->getTransform());
    this->p2Location = miniMapPlayerPosition(*g_scene.getEntity("player2")->getTransform());
    this->p3Location = miniMapPlayerPosition(*g_scene.getEntity("player3")->getTransform());
    this->p4Location = miniMapPlayerPosition(*g_scene.getEntity("player4")->getTransform());

    // Draw minimap icons
    renderImage(imageShader, p1Icon, p1Location.x, p1Location.y, 20.f, 20.f, 0, 1.f);
    renderImage(imageShader, p2Icon, p2Location.x, p2Location.y, 20.f, 20.f, 0, 1.f);
    renderImage(imageShader, p3Icon, p3Location.x, p3Location.y, 20.f, 20.f, 0, 1.f);
    renderImage(imageShader, p4Icon, p4Location.x, p4Location.y, 20.f, 20.f, 0, 1.f);

    // Draw minimap
    renderImage(imageShader, miniMap, scX(map_x), scY(0.8), scX(map_sx), scY(0.325), 0, 1.f);
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

glm::vec3 UISystem::offscreenBubbleLocation(int playerNum, glm::vec3 entityPos, int& vertical)
{
    vertical = 0;
    bool verticalOffscreen = false;
    
    glm::vec4 location = glm::vec4(entityPos, 1);

    glm::vec3 cam = g_scene.camera.centerBeam;
    glm::vec3 toEntity = entityPos - g_scene.camera.position;

    std::string player = "player" + std::to_string(playerNum);
    glm::mat4 viewMatrix = g_scene.camera.getViewMatrix(g_scene.getEntity(player)->getTransform());
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

        float perspective = 0.0f;

        switch (playerNum)
        {
        case 1:
            perspective = g_scene.p1Camera->getPerspective();
            break;
        case 2:
            perspective = g_scene.p1Camera->getPerspective();
            break;
        case 3:
            perspective = g_scene.p1Camera->getPerspective();
            break;
        case 4:
            perspective = g_scene.p1Camera->getPerspective();
            break;
        default:
            break; // Uh-oh, something has gone wrong
        }

        float viewRange = perspective / 2.0f;
        //float viewRange = 80 / 2.f;
        viewRange *= (PI / 180.0f);

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
