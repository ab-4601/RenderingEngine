#pragma once

#include "Window.h"
#include "Texture.h"

class Camera {
private:
	glm::vec3 position, front, up, right;
	glm::vec3 worldUpDir;

	GLuint cameraBuffer = 0;

	GLfloat pitch, yaw, roll;
	glm::mat4 projection;

	GLfloat movementSpeed, turnSpeed, movementSpeedMultiplier;

	void update();

public:
	Camera(glm::vec3 position = glm::vec3(0.f, 0.f, 3.f), int windowWidth = 800, int windowHeight = 600,
		glm::vec3 worldUpDir = glm::vec3(0.f, 1.f, 0.f),
		GLfloat pitch = 0.f, GLfloat yaw = 0.f, GLfloat moveSpeed = 3.f, GLfloat turnSpeed = 0.1f);

	void keyFunctionality(const Window* currWindow, GLfloat deltaTime);
	void mouseFunctionality(GLfloat xChange, GLfloat yChange, GLfloat scrollChange);

	inline glm::mat4 getProjectionMatrix() const { return this->projection; };

	inline glm::vec3 getCameraLookDirection() const {
		return this->front;
	}

	inline glm::vec3 getCameraPosition() const {
		return this->position;
	}

	glm::mat4 generateViewMatrix() const {
		return glm::lookAt(this->position, this->position + this->front, this->up);
	}

	~Camera() = default;
};