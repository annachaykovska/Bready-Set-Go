#pragma once

#include <string>

#include "Input.h"

class Window {
public:

	Window(int width, int height, std::string name);

	void swapBuffer() { glfwSwapBuffers(window); }
	int shouldClose() { return glfwWindowShouldClose(window); }
	GLFWwindow* getWindow() { return window; }
	void getInput();
	int getWidth() const { return width; }
	int getHeight() const { return height; }
	void clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }
	void setBackgroundColor(float r, float g, float b) { glClearColor(r, g, b, 1.0f); }

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