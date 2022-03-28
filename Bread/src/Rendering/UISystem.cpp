#include "UISystem.h"
#include "../Inventory.h"
#include "../Scene/Entity.h"
#include "../Gameplay/Recipe.h"

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
    , miniMap("resources/textures/map.png", GL_NEAREST)
    , inventory("resources/textures/inventory.png", GL_NEAREST)
    , tomato("resources/textures/tomato.png", GL_NEAREST)
    , cheese("resources/textures/cheese.png", GL_NEAREST)
    , sausage("resources/textures/sausage.png", GL_NEAREST)
    , dough("resources/textures/dough.png", GL_NEAREST)
    , cheeseOffscreen("resources/textures/cheeseOffscreen.png", GL_NEAREST)
    , tomatoOffscreen("resources/textures/tomatoOffscreen.png", GL_NEAREST)
    , doughOffscreen("resources/textures/doughOffscreen.png", GL_NEAREST)
    , sausageOffscreen("resources/textures/sausageOffscreen.png", GL_NEAREST)
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
    , speedometer_theta(MIN_SPEED_THETA)
    , gameOverPlayer1_1("resources/textures/game_over_screen_player_1.png", GL_NEAREST)
    , gameOverPlayer1_2("resources/textures/game_over_screen_player_1_2.png", GL_NEAREST)
    , gameOverPlayer1_3("resources/textures/game_over_screen_player_1_3.png", GL_NEAREST)
    , gameOverPlayer1_4("resources/textures/game_over_screen_player_1_4.png", GL_NEAREST)
    , gameOverPlayer2("resources/textures/game_over_screen_player_2.png", GL_NEAREST)
    , gameOverPlayer3("resources/textures/game_over_screen_player_3.png", GL_NEAREST)
    , gameOverPlayer4("resources/textures/game_over_screen_player_4.png", GL_NEAREST)
    , backToMainMenuButtonPressed("resources/textures/button_back_to_main_menu_selected.png", GL_NEAREST)
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

UISystem::~UISystem(){

}

void UISystem::updateMainMenu(int itemSelected) {
    // Buttons
    if (itemSelected == 1) { // start game
        renderImage(imageShader, exitButtonNormal, scX(0.79f), scY(0.47f), scX(0.2f), scY(0.1f), 0, 1.f);
        renderImage(imageShader, startGameButtonPressed, scX(0.79f), scY(0.60f), scX(0.2f), scY(0.1f), 0, 1.f);
    }
    else if (itemSelected == 2) { // exit
        renderImage(imageShader, exitButtonPressed, scX(0.79f), scY(0.47f), scX(0.2f), scY(0.1f), 0, 1.f);
        renderImage(imageShader, startGameButtonNormal, scX(0.79f), scY(0.60f), scX(0.2f), scY(0.1f), 0, 1.f);
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


void UISystem::updateGame(int endScreenValue) {
    if (checkForWin() != 0)
    {
        updateEndGame(endScreenValue);
    }
    float height, width;

    // Player 1 UI (eventually abstract to a draw player UI method)
    // Drawing speedometer
    float speedometer_goal_theta = lerp(std::min(std::max(abs(g_systems.physics->getPlayerSpeed(1)) / 40.f, 0.f), 1.f), MIN_SPEED_THETA, MAX_SPEED_THETA);
    speedometer_theta += std::min(std::max((speedometer_goal_theta-speedometer_theta),-MAX_NEEDLE_DELTA),MAX_NEEDLE_DELTA);
    renderImage(imageShader, needle, scX(0.875), scY(0.2), scX(0.225), scY(0.3), speedometer_theta, 1.f);

    renderImage(imageShader, speedometer, scX(0.875), scY(0.2), scX(0.225), scY(0.3), 0.f, 1.f);

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
        renderText(textShader, "(Omlette): " + std::to_string(p2Recipe->progress) + "/4", scX(0.83), scY(0.8), 1.f, glm::vec3(1, 1, 1));
        renderText(textShader, "(Wrap): " + std::to_string(p3Recipe->progress) + "/4", scX(0.83), scY(0.75), 1.f, glm::vec3(1, 1, 1));
        renderText(textShader, "(Salad): " + std::to_string(p4Recipe->progress) + "/4", scX(0.83), scY(0.7), 1.f, glm::vec3(1, 1, 1));
    }

    // Drawing minimap
    renderImage(imageShader, p1Icon, p1Location.x, p1Location.y, 20.f, 20.f, 0, 1.f);
    renderImage(imageShader, p2Icon, p2Location.x, p2Location.y, 20.f, 20.f, 0, 1.f);
    renderImage(imageShader, p3Icon, p3Location.x, p3Location.y, 20.f, 20.f, 0, 1.f);
    renderImage(imageShader, p4Icon, p4Location.x, p4Location.y, 20.f, 20.f, 0, 1.f);

    renderImage(imageShader, miniMap, scX(0.125), scY(0.8), scX(0.15625), scY(0.3), 0, 1.f);

    glm::vec3 IngLocation;

    Entity* player1 = g_scene.getEntity("player1");
    Inventory* p1Inv = (Inventory*)player1->getComponent("inventory");

    if (!p1Inv->cheese)
    {
        IngLocation = offscreenBubbleLocation(tracker->getCheeseLocation().position);
        renderImage(imageShader, cheeseOffscreen, IngLocation.x, IngLocation.y, IngLocation.z, IngLocation.z, 0, 1.f);
    }

    if (!p1Inv->tomato)
    {
        IngLocation = offscreenBubbleLocation(tracker->getTomatoLocation().position);
        renderImage(imageShader, tomatoOffscreen, IngLocation.x, IngLocation.y, IngLocation.z, IngLocation.z, 0, 1.f);
    }

    if (!p1Inv->dough)
    {
        IngLocation = offscreenBubbleLocation(tracker->getDoughLocation().position);
        renderImage(imageShader, doughOffscreen, IngLocation.x, IngLocation.y, IngLocation.z, IngLocation.z, 0, 1.f);
    }

    if (!p1Inv->sausage)
    {
        IngLocation = offscreenBubbleLocation(tracker->getSausageLocation().position);
        renderImage(imageShader, sausageOffscreen, IngLocation.x, IngLocation.y, IngLocation.z, IngLocation.z, 0, 1.f);
    }

    // Drawing Inventory
    float alpha;
    float faded = 0.4f;
    float opaque = 1.f;

    float invScale = scX(0.04);
    float invXOffset = scX(0.12);
    float invYOffset = scY(0.4);
    float invBlockOffset = scY(0.097);
    float recipeYOffset = scY(0.55);

    alpha = (p1Inv->tomato) ? opaque : faded;
    renderImage(imageShader, tomato, invXOffset, invYOffset - (0 * invBlockOffset), invScale, invScale, 0, alpha);

    alpha = (p1Inv->cheese) ? opaque : faded;
    renderImage(imageShader, cheese, invXOffset, invYOffset - (1 * invBlockOffset), invScale, invScale, 0, alpha);
    
    alpha = (p1Inv->dough) ? opaque : faded;
    renderImage(imageShader, dough, invXOffset, invYOffset - (2 * invBlockOffset), invScale, invScale, 0, alpha);

    alpha = (p1Inv->sausage) ? opaque : faded;
    renderImage(imageShader, sausage, invXOffset, invYOffset - (3 * invBlockOffset), invScale, invScale, 0, alpha);

    alpha = (p1Inv->tomato && p1Inv->cheese && p1Inv->dough && p1Inv->sausage) ? opaque : faded;
    renderImage(imageShader, pizza, invXOffset, recipeYOffset, scX(0.06), scX(0.06), 0, alpha);

    renderImage(imageShader, inventory, invXOffset, scY(0.33), scX(0.1), scX(0.32), 0, 1.f);

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

    if (p2Inv->rice && p2Inv->chicken && p2Inv->dough && p2Inv->lettuce)
    {
        return 2;
    }

    Entity* player3 = g_scene.getEntity("player3");
    Inventory* p3Inv = (Inventory*)player3->getComponent("inventory");

    if (p3Inv->lettuce && p3Inv->tomato && p3Inv->cheese && p3Inv->carrot)
    {
        return 3;
    }

    Entity* player4 = g_scene.getEntity("player4");
    Inventory* p4Inv = (Inventory*)player4->getComponent("inventory");

    if (p4Inv->egg && p4Inv->lettuce && p4Inv->cheese && p4Inv->peas)
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
    std::cout << g_systems.width << std::endl;
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

glm::vec3 UISystem::offscreenBubbleLocation(glm::vec3 entityPos)
{
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

    if (location.z < -1 || location.z > 1 || location.x > g_systems.width || location.x < 0)
    {
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
            float offset = ((((g_systems.height / 2.f) - 0) / ((PI / 2.f) - viewRange)) * (yawTheta - viewRange));
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
