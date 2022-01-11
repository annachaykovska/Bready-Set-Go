#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>

#include "Window.h"
#include "Geometry.h"
#include "Shader.h"
#include "Texture.h"

int main()
{	
	// Initialize GLFW library
	glfwInit();

	// Create viewport window
	Window window(800, 600, "Bread");

	CPU_Geometry rectangle;
	rectangle.verts.push_back(glm::vec3(0.5f, 0.5f, 0.0f));
	rectangle.verts.push_back(glm::vec3(0.5f, -0.5f, 0.0f));
	rectangle.verts.push_back(glm::vec3(-0.5f, -0.5f, 0.0f));
	rectangle.verts.push_back(glm::vec3(-0.5f, 0.5f, 0.0f));
	rectangle.colors.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
	rectangle.colors.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	rectangle.colors.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	rectangle.colors.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
	rectangle.texCoords.push_back(glm::vec2(1.0f, 1.0f));
	rectangle.texCoords.push_back(glm::vec2(1.0f, 0.0f));
	rectangle.texCoords.push_back(glm::vec2(0.0f, 0.0f));
	rectangle.texCoords.push_back(glm::vec2(0.0f, 1.0f));
	rectangle.indices.push_back(glm::ivec3(0, 1, 3));
	rectangle.indices.push_back(glm::ivec3(1, 2, 3));

	CPU_Geometry twoTriangles;
	twoTriangles.verts.push_back(glm::vec3(-0.75f, -0.75f, 0.0f));
	twoTriangles.verts.push_back(glm::vec3(-0.5f, 0.0f, 0.0f));
	twoTriangles.verts.push_back(glm::vec3(-0.25f, -0.75f, 0.0f));
	twoTriangles.verts.push_back(glm::vec3(0.25f, 0.0f, 0.0f));
	twoTriangles.verts.push_back(glm::vec3(0.5f, 0.75f, 0.0f));
	twoTriangles.verts.push_back(glm::vec3(0.75f, 0.0f, 0.0f));
	twoTriangles.colors.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
	twoTriangles.colors.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
	twoTriangles.colors.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
	twoTriangles.colors.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	twoTriangles.colors.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	twoTriangles.colors.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	twoTriangles.indices.push_back(glm::ivec3(0, 1, 2));
	twoTriangles.indices.push_back(glm::ivec3(3, 4, 5));
	
	// Load object to gpu
	GPU_Geometry renderable(rectangle);
	renderable.texture.setTexture("resources/textures/container.jpg");
	//GPU_Geometry renderable2(twoTriangles);
	//renderable.upload(rectangle);

	// Compile shader program
	Shader shader("resources/shaders/vertex.txt", "resources/shaders/fragment.txt");

	// Set background color
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	// GAME LOOP
	while (!window.shouldClose()) 
	{
		// INPUT
		window.getInput();
		glfwPollEvents();

		// SIMULATE

		// RENDER
		glClear(GL_COLOR_BUFFER_BIT);
		shader.use();
		renderable.draw();
		//renderable2.draw();
		window.swapBuffer();

		// AUDIO
	}

	renderable.cleanup();
	shader.cleanup();
	glfwTerminate();

	return 0;
}