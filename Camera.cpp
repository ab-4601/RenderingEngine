#include "Camera.h"

Camera::Camera(glm::vec3 position, int windowWidth, int windowHeight, glm::vec3 worldUpDir,
	GLfloat pitch, GLfloat yaw, GLfloat moveSpeed, GLfloat turnSpeed) :
	position{ position }, worldUpDir{ worldUpDir }, pitch{ pitch }, yaw{ yaw },
	movementSpeed{ moveSpeed }, turnSpeed{ turnSpeed }, movementSpeedMultiplier{ 1.f } 
{

	glm::vec3 origin(0.f, 0.f, 0.f);
	front = glm::normalize(this->position - origin);

	float aspect = (float)windowWidth / windowHeight;
	projection = glm::perspective(glm::radians(45.f), aspect, ::near_plane, ::far_plane);

	glGenBuffers(1, &cameraBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, cameraBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4x4) * 2 + sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, cameraBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	update();
}

void Camera::keyFunctionality(const Window* currWindow, GLfloat deltaTime) {
	GLfloat velocity = movementSpeed * deltaTime * movementSpeedMultiplier;

	if (currWindow->getKeyPress(GLFW_KEY_W) || currWindow->getKeyPress(GLFW_KEY_UP))
		position += front * velocity;

	if (currWindow->getKeyPress(GLFW_KEY_S) || currWindow->getKeyPress(GLFW_KEY_DOWN))
		position -= front * velocity;

	if (currWindow->getKeyPress(GLFW_KEY_D) || currWindow->getKeyPress(GLFW_KEY_RIGHT))
		position += right * velocity;

	if (currWindow->getKeyPress(GLFW_KEY_A) || currWindow->getKeyPress(GLFW_KEY_LEFT))
		position -= right * velocity;

	if (currWindow->getKeyPress(GLFW_KEY_SPACE))
		position += up * velocity;

	if (currWindow->getKeyPress(GLFW_KEY_LEFT_CONTROL))
		position -= up * velocity;

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

	glm::mat4x4 cameraSpaceMatrices[2] = { projection, generateViewMatrix() };

	glBindBuffer(GL_UNIFORM_BUFFER, cameraBuffer);
	for (int i = 0; i < 3; i++) {
		if (i != 2)
			glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(glm::mat4x4), sizeof(glm::mat4x4), &cameraSpaceMatrices[i]);
		else
			glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(glm::mat4x4), sizeof(glm::vec3), &position);
	}
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}