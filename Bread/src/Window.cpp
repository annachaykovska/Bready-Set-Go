#include <iostream>

#include <glad/glad.h>
#include <glfw/glfw3.h>

#include "Window.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

Window::Window(int width, int height, std::string title)
	: width(width), height(height), title(title)
{
	setup();
	window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
	verifyGLFW();

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	verifyGLAD();
};

void Window::getInput() 
{
	input.processInput(window);
}

void Window::setup() {

	// Define window settings
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
}

int Window::verifyGLFW() 
{
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window." << std::endl;
		glfwTerminate();
		return -1;
	}

	return 1;
}

int Window::verifyGLAD() 
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	return 1;
}