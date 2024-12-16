#pragma once

#include "Core.h"

class Window {

private:
	// Window variables
	GLFWwindow* window;
	unsigned int windowWidth, windowHeight;
	int bufferWidth, bufferHeight;

	// Camera variables
	double viewportX, viewportY;
	GLfloat lastX, lastY, XChange, YChange;
	bool mouseFirstMoved;

	GLfloat scrollChange;

	bool keys[1024];
	bool LMBPressed;
	bool RMBPressed;

	// Camera movement and setup
	static void handleInput(GLFWwindow* window, int key, int code, int action, int mode);
	static void setMousePosition(GLFWwindow* window, double xPos, double yPos);
	static void setMouseScroll(GLFWwindow* window, double xOffset, double yOffset);
	static void getMouseButton(GLFWwindow* window, int button, int action, int mods);

	inline void createCallbacks() {
		glfwSetKeyCallback(this->window, handleInput);
		glfwSetCursorPosCallback(this->window, setMousePosition);
		glfwSetScrollCallback(this->window, setMouseScroll);
		glfwSetMouseButtonCallback(this->window, getMouseButton);
	}

public:
	Window();

	// Setup window
	void setupWindow();
	GLFWwindow* getMainWindow() { return this->window; }

	// Get buffer dimensions
	inline int getBufferWidth() const { return this->bufferWidth; }
	inline int getBufferHeight() const { return this->bufferHeight; }

	inline unsigned int getWindowWidth() const { return this->windowWidth; }
	inline unsigned int getWindowHeight() const { return this->windowHeight; }

	inline bool getKeyPress(unsigned int keyVal) const { return this->keys[keyVal]; }

	inline GLFWwindow* getGlfwWindow() const { return this->window; }
	inline const Window* getCurrWindow() const { return this; }

	inline void setLMBPressed(bool val) { this->LMBPressed = val; }
	inline void setRMBPressed(bool val) { this->RMBPressed = val; }

	inline bool getLMBPressed() const { return this->LMBPressed; }
	inline bool getRMBPressed() const { return this->RMBPressed; }

	inline glm::vec2 getViewportCoord() const { return glm::vec2(this->viewportX, this->viewportY); }

	GLfloat getXChange() {
		GLfloat delta = this->XChange;
		this->XChange = 0.f;
		return delta;
	}

	GLfloat getYChange() {
		GLfloat delta = this->YChange;
		this->YChange = 0.f;
		return delta;
	}

	GLfloat getScrollChange() {
		GLfloat delta = this->scrollChange;
		this->scrollChange = 0.f;
		return delta;
	}

	~Window() {
		glDisable(GL_DEPTH_TEST);

		glfwDestroyWindow(this->window);
		glfwTerminate();
	}
};