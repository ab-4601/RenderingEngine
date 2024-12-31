#pragma once

#include "Window.h"
#include "Texture.h"

struct CameraData {
	glm::mat4 projection{ 1.f };
	glm::mat4 view{ 1.f };
	glm::vec3 position{ 0.f };
};

class Camera {
private:
	glm::vec3 front, up, right;
	glm::vec3 worldUpDir;

	GLuint cameraBuffer = 0;

	GLfloat pitch, yaw, roll;

	GLfloat movementSpeed, turnSpeed, movementSpeedMultiplier;
	CameraData cameraData;

	void update();

public:
	Camera(glm::vec3 position = glm::vec3(0.f, 0.f, 3.f), int windowWidth = 800, int windowHeight = 600,
		glm::vec3 worldUpDir = glm::vec3(0.f, 1.f, 0.f), GLfloat pitch = 0.f, GLfloat yaw = 0.f,
		GLfloat moveSpeed = 3.f, GLfloat turnSpeed = 0.1f);

	void keyFunctionality(const Window* currWindow, GLfloat deltaTime);
	void mouseFunctionality(GLfloat xChange, GLfloat yChange, GLfloat scrollChange);

	glm::mat4 getProjectionMatrix() const { return cameraData.projection; };
	glm::vec3 getCameraLookDirection() const { return front; }
	glm::vec3 getCameraPosition() const { return cameraData.position; }

	glm::mat4 generateViewMatrix() const { return glm::lookAt(cameraData.position, cameraData.position + front, up); }

	~Camera() = default;
};