#pragma once

#include <string>

#include "Input.h"

class CallbackInterface {
public:
	virtual void keyCallback(int key, int scancode, int action, int mods) {}
	virtual void mouseButtonCallback(int button, int action, int mods) {}
	virtual void cursorPosCallback(double xpos, double ypos) {}
	virtual void scrollCallback(double xoffset, double yoffset) {}
	virtual void windowSizeCallback(int width, int height) { glViewport(0, 0, width, height); }
};

// Functor for deleting a GLFW window.
//
// This is used as a custom deleter with std::unique_ptr so that the window
// is properly destroyed when std::unique_ptr needs to clean up its resource
struct WindowDeleter {
	void operator() (GLFWwindow* window) const {
		glfwDestroyWindow(window);
	}
};

class Window {
public:

	Window(std::shared_ptr<CallbackInterface> callbacks, int width, int height, std::string name);
	Window(int width, int height, std::string name);

	void setCallbacks(std::shared_ptr<CallbackInterface> callbacks);

	void swapBuffer() { glfwSwapBuffers(window.get()); }
	int shouldClose() { return glfwWindowShouldClose(window.get()); }
	GLFWwindow* getWindow() { return window.get(); }
	void getInput();
	int getWidth() const { return width; }
	int getHeight() const { return height; }
	void clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }
	void setBackgroundColor(float r, float g, float b) { glClearColor(r, g, b, 1.0f); }

private:
	std::unique_ptr<GLFWwindow, WindowDeleter> window;
	std::shared_ptr<CallbackInterface> callbacks;      // optional shared owning ptr (user provided)

	void setup();
	int verifyGLFW();
	int verifyGLAD();
	void connectCallbacks();

	int width;
	int height;
	std::string title;
	Input input;

	static void keyMetaCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouseButtonMetaCallback(GLFWwindow* window, int button, int action, int mods);
	static void cursorPosMetaCallback(GLFWwindow* window, double xpos, double ypos);
	static void scrollMetaCallback(GLFWwindow* window, double xoffset, double yoffset);
	static void windowSizeMetaCallback(GLFWwindow* window, int width, int height);
};
