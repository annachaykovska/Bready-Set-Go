#include "UISystem.h"

UISystem::UISystem() 
    : textShader("resources/shaders/textVertex.txt", "resources/shaders/textFragment.txt") 
    , imageShader("resources/shaders/imageVertex.txt", "resources/shaders/imageFragment.txt")
    , speedometer("resources/textures/speed.png", GL_NEAREST)
{

    textShader.checkCompileErrors(textShader.getId(), "PROGRAM");
    projection = glm::ortho(0.f, 800.f, 0.f, 600.f); //check if the max limits can be changed
	//Freetype initialization
	if (FT_Init_FreeType(&ft))
	{
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		return;
	}
	if (FT_New_Face(ft, "resources/fonts/OpenSans.ttf", 0, &face))
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

    // Not sure if this is needed
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //

    // Binding the VAO and VBO
    // CHECK: what parts need to be done per frame
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

	std::cout << "UI initialized" << std::endl;
}

UISystem::~UISystem(){
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

void UISystem::update() {
    renderText(textShader, "Bready Set Go", 25.0f, 540.0f, 1.0f, glm::vec3(0.5, 0.5f, 0.5f));
    float height = speedometer.height;
    float width = speedometer.width;
    renderImage(imageShader, speedometer, 560.0f, 5.0f, 200.0f * (width / height), 200.0f * (height / width));
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