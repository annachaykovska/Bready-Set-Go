#pragma once

#include <string>

#include "Input.h"

class Window {
public:

	Window(int width, int height, std::string name);

	void swapBuffer() { glfwSwapBuffers(window); }
	int shouldClose() { return glfwWindowShouldClose(window); }
	void getInput();

	int getWidth() const { return width; }
	int getHeight() const { return height; }

private:

	void setup();
	int verifyGLFW();
	int verifyGLAD();

	int width;
	int height;
	std::string title;
	GLFWwindow* window;
	Input input;
};