#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Window.h"
#include "Profiler.h"
#include "Geometry.h"
#include "Shader.h"
#include "Texture.h"
#include "Model.h"

#include <stbi/stb_image.h>

int main()
{	
	// Initialize GLFW library
	glfwInit();

	// Create viewport window
	Window window(800, 600, "Bread");
	window.setBackgroundColor(0.2f, 0.3f, 0.3f);

	// Make ground plane
	std::vector<Vertex> verts;
	Vertex g0, g1, g2, g3;
	g0.position = glm::vec3(-1.0f, 0.0f, -1.0f);
	g0.normal = glm::vec3(0.0f, 1.0f, 0.0f);
	g0.texCoords = glm::vec2(0.0f, 0.0f);
	verts.push_back(g0);
	g1.position = glm::vec3(1.0f, 0.0f, -1.0f);
	g1.normal = glm::vec3(0.0f, 1.0f, 0.0f);
	g1.texCoords = glm::vec2(1.0f, 0.0f);
	verts.push_back(g1);
	g2.position = glm::vec3(1.0f, 0.0f, 1.0f);
	g2.normal = glm::vec3(0.0f, 1.0f, 0.0f);
	g2.texCoords = glm::vec2(1.0f, 1.0f);
	verts.push_back(g2);
	g3.position = glm::vec3(-1.0f, 0.0f, 1.0f);
	g3.normal = glm::vec3(0.0f, 1.0f, 0.0f);
	g3.texCoords = glm::vec2(0.0f, 1.0f);
	verts.push_back(g3);
	std::vector<unsigned int> inds;
	inds.push_back(0);
	inds.push_back(1);
	inds.push_back(2);
	inds.push_back(0);
	inds.push_back(2);
	inds.push_back(3);
	std::vector<Texture> texs;
	unsigned int textureId;
	glGenTextures(1, &textureId);
	int width, height, nrChannels;
	unsigned char* data = stbi_load("resources/textures/diffuse.jpg", &width, &height, &nrChannels, 0);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
	Texture groundTexture;
	groundTexture.id = textureId;
	texs.push_back(groundTexture);
	Mesh ground(verts, inds, texs);

	// Compile shader program
	Shader shader("resources/shaders/vertex.txt", "resources/shaders/fragment.txt");

	// ImGui profiler for debugging
	Profiler profiler(window);

	// Load bread model
	std::string breadPath = "resources/models/bread/PB145_bread_low.obj";
	Model test(&breadPath[0]);

	const float radius = 40.0f;

	glEnable(GL_DEPTH_TEST);

	// GAME LOOP
	while (!window.shouldClose()) 
	{
		// INPUT
		window.getInput();
		glfwPollEvents();

		// SIMULATE

		// RENDER
		window.clear();
		profiler.run();
		shader.use();

		// MVP matrices
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 5.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-70.00f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));

		glm::mat4 view = glm::mat4(1.0f);
		float camX = sin(glfwGetTime()) * radius;
		float camZ = cos(glfwGetTime()) * radius;
		view = glm::lookAt(glm::vec3(camX, 10.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		glm::mat4 proj = glm::mat4(1.0f);
		proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 1000.0f);

		unsigned int texLoc = glGetUniformLocation(shader.getId(), "textured");
		glUniform1i(texLoc, 0);

		unsigned int modelLoc = glGetUniformLocation(shader.getId(), "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		unsigned int viewLoc = glGetUniformLocation(shader.getId(), "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		unsigned int projLoc = glGetUniformLocation(shader.getId(), "proj");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

		test.draw(shader);

		//glUniform1i(texLoc, 1);

		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(100.0f, 100.0f, 100.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		ground.draw(shader);

		window.swapBuffer();

		// AUDIO
	}

	profiler.cleanup();
	shader.cleanup();
	glfwTerminate();

	return 0;
}