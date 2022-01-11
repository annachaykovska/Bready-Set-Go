#include <glfw/glfw3.h>

#include "Input.h"

void Input::processInput(GLFWwindow* window) 
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_UP))
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (glfwGetKey(window, GLFW_KEY_DOWN))
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}