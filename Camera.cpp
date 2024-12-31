#include "Camera.h"

Camera::Camera(glm::vec3 position, int windowWidth, int windowHeight, glm::vec3 worldUpDir,
	GLfloat pitch, GLfloat yaw, GLfloat moveSpeed, GLfloat turnSpeed) :
	worldUpDir{ worldUpDir }, pitch{ pitch }, yaw{ yaw },
	movementSpeed{ moveSpeed }, turnSpeed{ turnSpeed }, movementSpeedMultiplier{ 1.f } 
{
	cameraData.position = position;

	glm::vec3 origin(0.f, 0.f, 0.f);
	front = glm::normalize(cameraData.position - origin);

	float aspect = (float)windowWidth / windowHeight;
	cameraData.projection = glm::perspective(glm::radians(45.f), aspect, ::near_plane, ::far_plane);
	cameraData.view = generateViewMatrix();

	glGenBuffers(1, &cameraBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, cameraBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(CameraData), NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, cameraBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	update();
}

void Camera::keyFunctionality(const Window* currWindow, GLfloat deltaTime) {
	GLfloat velocity = movementSpeed * deltaTime * movementSpeedMultiplier;

	if (currWindow->getKeyPress(GLFW_KEY_W) || currWindow->getKeyPress(GLFW_KEY_UP))
		cameraData.position += front * velocity;

	if (currWindow->getKeyPress(GLFW_KEY_S) || currWindow->getKeyPress(GLFW_KEY_DOWN))
		cameraData.position -= front * velocity;

	if (currWindow->getKeyPress(GLFW_KEY_D) || currWindow->getKeyPress(GLFW_KEY_RIGHT))
		cameraData.position += right * velocity;

	if (currWindow->getKeyPress(GLFW_KEY_A) || currWindow->getKeyPress(GLFW_KEY_LEFT))
		cameraData.position -= right * velocity;

	if (currWindow->getKeyPress(GLFW_KEY_SPACE))
		cameraData.position += up * velocity;

	if (currWindow->getKeyPress(GLFW_KEY_LEFT_CONTROL))
		cameraData.position -= up * velocity;

	if (currWindow->getKeyPress(GLFW_KEY_LEFT_SHIFT))
		movementSpeedMultiplier = 3.f;
	else
		movementSpeedMultiplier = 1.f;
}

void Camera::mouseFunctionality(GLfloat xChange, GLfloat yChange, GLfloat scrollChange) {
	if (movementSpeed > 0)
		movementSpeed += scrollChange * 100;

	if (movementSpeed <= 0)
		movementSpeed = 1.f;

	xChange *= turnSpeed;
	yChange *= turnSpeed;

	yaw += xChange;
	pitch += yChange;

	if (pitch > 89.f) {
		pitch = 89.f;
	}

	if (pitch < -89.f) {
		pitch = -89.f;
	}

	update();
}

void Camera::update() {
	front.x = cosf(glm::radians(yaw)) * cosf(glm::radians(pitch));
	front.y = sinf(glm::radians(pitch));
	front.z = sinf(glm::radians(yaw)) * cosf(glm::radians(pitch));

	front = glm::normalize(front);

	right = glm::normalize(glm::cross(front, worldUpDir));
	up = glm::normalize(glm::cross(right, front));

	cameraData.view = generateViewMatrix();

	GLint bufferSize{ 0 };

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, cameraBuffer);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(CameraData), &cameraData);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}