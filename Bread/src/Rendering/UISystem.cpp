#include "UISystem.h"
#include "../Inventory.h"
#include "../Scene/Entity.h"

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
    , mainMenuBackground("resources/textures/main_menu_7.png", GL_NEAREST)
    , startGameButtonNormal("resources/textures/button_start_game_2.png", GL_NEAREST)
    , startGameButtonPressed("resources/textures/button_start_game_selected_2.png", GL_NEAREST)
    , exitButtonNormal("resources/textures/button_exit_2.png", GL_NEAREST)
    , exitButtonPressed("resources/textures/button_exit_selected_2.png", GL_NEAREST)
    , speedometer_theta(MIN_SPEED_THETA)
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
	if (FT_New_Face(ft, "resources/fonts/arial.ttf", 0, &face))
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


void UISystem::updateGame() {
    
    if (checkForWin() != 0)
    {
        std::string winText = "Player " + std::to_string(checkForWin()) + " Wins!";
        renderText(textShader, winText, 250.0f, 500.0f, 1.0f, glm::vec3(0.5, 0.5f, 0.5f));
    }
    float height, width;

    // Player 1 UI (eventually abstract to a draw player UI method)
    // Drawing speedometer
    float speedometer_goal_theta = lerp(std::min(std::max(abs(g_systems.physics->getPlayerSpeed(1)) / 50.f, 0.f), 1.f), MIN_SPEED_THETA, MAX_SPEED_THETA);
    speedometer_theta += std::min(std::max((speedometer_goal_theta-speedometer_theta),-MAX_NEEDLE_DELTA),MAX_NEEDLE_DELTA);
    renderImage(imageShader, needle, scX(0.875), scY(0.2), scX(0.225), scY(0.3), speedometer_theta, 1.f);

    renderImage(imageShader, speedometer, scX(0.875), scY(0.2), scX(0.225), scY(0.3), 0.f, 1.f);


    // Drawing minimap
    renderImage(imageShader, p1Icon, p1Location.x, p1Location.y, scX(0.01875), scX(0.01875), 0, 1.f);
    renderImage(imageShader, p2Icon, p2Location.x, p2Location.y, scX(0.01875), scX(0.01875), 0, 1.f);
    renderImage(imageShader, p3Icon, p3Location.x, p3Location.y, scX(0.01875), scX(0.01875), 0, 1.f);
    renderImage(imageShader, p4Icon, p4Location.x, p4Location.y, scX(0.01875), scX(0.01875), 0, 1.f);

    renderImage(imageShader, miniMap, scX(0.125), scY(0.8), scX(0.15), scX(0.15), 0, 1.f);

    glm::vec3 IngLocation;
    IngLocation = offscreenBubbleLocation(tracker->getCheeseLocation().position);
    renderImage(imageShader, cheeseOffscreen, IngLocation.x, IngLocation.y, IngLocation.z, IngLocation.z, 0, 1.f);

    IngLocation = offscreenBubbleLocation(tracker->getTomatoLocation().position);
    renderImage(imageShader, tomatoOffscreen, IngLocation.x, IngLocation.y, IngLocation.z, IngLocation.z, 0, 1.f);

    IngLocation = offscreenBubbleLocation(tracker->getDoughLocation().position);
    renderImage(imageShader, doughOffscreen, IngLocation.x, IngLocation.y, IngLocation.z, IngLocation.z, 0, 1.f);

    IngLocation = offscreenBubbleLocation(tracker->getSausageLocation().position);
    renderImage(imageShader, sausageOffscreen, IngLocation.x, IngLocation.y, IngLocation.z, IngLocation.z, 0, 1.f);

    // Drawing Inventory
    Entity* player1 = g_scene.getEntity("player1");
    Inventory* p1Inv = (Inventory*)player1->getComponent("inventory");
    float alpha;
    float faded = 0.2f;
    float opaque = 1.f;

    float invScale = scX(0.04);
    float invXOffset = scX(0.12);
    float invYOffset = scY(0.4);
    float invBlockOffset = scY(0.097);
    float recipeYOffset = scY(0.54);

    alpha = (p1Inv->tomato) ? opaque : faded;
    renderImage(imageShader, tomato, invXOffset, invYOffset - (0 * invBlockOffset), invScale, invScale, 0, alpha);

    alpha = (p1Inv->cheese) ? opaque : faded;
    renderImage(imageShader, cheese, invXOffset, invYOffset - (1 * invBlockOffset), invScale, invScale, 0, alpha);
    
    alpha = (p1Inv->dough) ? opaque : faded;
    renderImage(imageShader, dough, invXOffset, invYOffset - (2 * invBlockOffset), invScale, invScale, 0, alpha);

    alpha = (p1Inv->sausage) ? opaque : faded;
    renderImage(imageShader, sausage, invXOffset, invYOffset - (3 * invBlockOffset), invScale, invScale, 0, alpha);

    alpha = (p1Inv->tomato && p1Inv->cheese && p1Inv->dough && p1Inv->sausage) ? opaque : faded;
    renderImage(imageShader, pizza, invXOffset, recipeYOffset, invScale, invScale, 0, alpha);

    renderImage(imageShader, inventory, invXOffset, scY(0.33), scX(0.1), scX(0.3), 0, 1.f);

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

    if (p2Inv->tomato && p2Inv->cheese && p2Inv->dough && p2Inv->sausage)
    {
        return 2;
    }

    Entity* player3 = g_scene.getEntity("player3");
    Inventory* p3Inv = (Inventory*)player3->getComponent("inventory");

    if (p3Inv->tomato && p3Inv->cheese && p3Inv->dough && p3Inv->sausage)
    {
        return 3;
    }

    Entity* player4 = g_scene.getEntity("player4");
    Inventory* p4Inv = (Inventory*)player4->getComponent("inventory");

    if (p4Inv->tomato && p4Inv->cheese && p4Inv->dough && p4Inv->sausage)
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
    // TODO: Fix mini map with new kitchen layout
    float x;
    float z;
    glm::vec2 location;
    x = ((150.f) / (150.f + 310.f)) * (transform.position.x + 310.f);
    z = ((150.f) / (220.f + 235.f)) * (transform.position.z + 235.f);
    location = glm::vec2(x, z);
    location.y = 200.f - location.y;
    location += glm::vec2(25.f, 380.f);
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
    }

    location.z = scX(0.07) + ((scX(0.02) - scX(0.07)) / (400.f - 0.f)) * (length(toEntity) - 0);
    if (location.z < scX(0.02))
    {
        location.z = scX(0.02);
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