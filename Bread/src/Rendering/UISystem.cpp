#include "UISystem.h"

UISystem::UISystem() 
    : textShader("resources/shaders/textVertex.txt", "resources/shaders/textFragment.txt") 
    , imageShader("resources/shaders/imageVertex.txt", "resources/shaders/imageFragment.txt")
    , speedometer("resources/textures/speedometer.png", GL_NEAREST)
    , needle("resources/textures/speedNeedle.png", GL_NEAREST)
    , miniMap("resources/textures/map.png", GL_NEAREST)
    , inventory("resources/textures/inventory.png", GL_NEAREST)
    , p1Icon("resources/textures/p1Icon.png", GL_NEAREST)
    , p2Icon("resources/textures/p2Icon.png", GL_NEAREST)
    , p3Icon("resources/textures/p3Icon.png", GL_NEAREST)
    , p4Icon("resources/textures/p4Icon.png", GL_NEAREST)
    , p1Location(glm::vec2(0))
    , p2Location(glm::vec2(0))
    , p3Location(glm::vec2(0))
    , p4Location(glm::vec2(0))
{
    //Variables needed to initialize freetype characters
    FT_Library ft;
    FT_Face face;

    textShader.checkCompileErrors(textShader.getId(), "PROGRAM");
    projection = glm::ortho(0.f, 800.f, 0.f, 600.f); //check if the max limits can be changed
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
	//std::cout << "UI initialized" << std::endl;
}

UISystem::~UISystem(){

}

void UISystem::update() {
    //renderText(textShader, "Bready Set Go", 25.0f, 500.0f, 1.0f, glm::vec3(0.5, 0.5f, 0.5f));
    float height, width;

    height = speedometer.height;
    width = speedometer.width;
    renderImage(imageShader, speedometer, 600.0f, 40.0f, 180.0f * (width / height), 180.0f * (height / width));

    height = needle.height;
    width = needle.width;
    renderImage(imageShader, needle, 685.0f, -5.0f, 200.0f * (width / height), 8.0f * (height / width));

    height = miniMap.height;
    width = miniMap.width;
    renderImage(imageShader, miniMap, 25.0f, 430.0f, 150.0f * (width / height), 150.0f * (height / width));

    height = p1Icon.height;
    width = p1Icon.width;
    renderImage(imageShader, p1Icon, p1Location.x, p1Location.y, 15.0f * (width / height), 15.0f * (height / width));
    height = p2Icon.height;
    width = p2Icon.width;
    renderImage(imageShader, p2Icon, p2Location.x, p2Location.y, 15.0f * (width / height), 15.0f * (height / width));
    height = p3Icon.height;
    width = p3Icon.width;
    renderImage(imageShader, p3Icon, p3Location.x, p3Location.y, 15.0f * (width / height), 15.0f * (height / width));
    height = p4Icon.height;
    width = p4Icon.width;
    renderImage(imageShader, p4Icon, p4Location.x, p4Location.y, 15.0f * (width / height), 15.0f * (height / width));

    height = inventory.height;
    width = inventory.width;
    renderImage(imageShader, inventory, 50.0f, 40.0f, 400.0f * (width / height), 80.0f * (height / width));
}

void UISystem::updateMiniMap(Transform& p1Transform, Transform& p2Transform, Transform& p3Transform, Transform& p4Transform)
{
    // TODO: Clean up these hardcoded numbers
    float x;
    float z;
    x = ((150.f) / (150.f + 310.f)) * (p1Transform.position.x + 310.f);
    z = ((150.f) / (220.f + 235.f)) * (p1Transform.position.z + 235.f);
    p1Location = glm::vec2(x, z);
    p1Location.y = 200.f - p1Location.y;
    p1Location += glm::vec2(25.f, 380.f);

    x = ((150.f) / (150.f + 310.f)) * (p2Transform.position.x + 310.f);
    z = ((150.f) / (220.f + 235.f)) * (p2Transform.position.z + 235.f);
    p2Location = glm::vec2(x, z);
    p2Location.y = 200.f - p2Location.y;
    p2Location += glm::vec2(25.f, 380.f);

    x = ((150.f) / (150.f + 310.f)) * (p3Transform.position.x + 310.f);
    z = ((150.f) / (220.f + 235.f)) * (p3Transform.position.z + 235.f);
    p3Location = glm::vec2(x, z);
    p3Location.y = 200.f - p3Location.y;
    p3Location += glm::vec2(25.f, 380.f);

    x = ((150.f) / (150.f + 310.f)) * (p4Transform.position.x + 310.f);
    z = ((150.f) / (220.f + 235.f)) * (p4Transform.position.z + 235.f);
    p4Location = glm::vec2(x, z);
    p4Location.y = 200.f - p4Location.y;
    p4Location += glm::vec2(25.f, 380.f);
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

        //std::cout << "print character";

        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void UISystem::renderImage(Shader& s, ImageTexture& image, float x, float y, float scaleX, float scaleY)
{
    s.use();
    glUniformMatrix4fv(glGetUniformLocation(s.getId(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    float vertices[6][4] = {
            { x,     y + scaleY,   0.0f, 1.0f },
            { x,     y,       0.0f, 0.f },
            { x + scaleX, y,       1.f, 0.f },

            { x,     y + scaleY,   0.0f, 1.0f },
            { x + scaleX, y,       1.f, 0.f },
            { x + scaleX, y + scaleY,   1.f, 1.0f }
    };

    // render glyph texture over quad
    image.bind();
    // update content of VBO memory
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // render quad
    glDrawArrays(GL_TRIANGLES, 0, 6);
}