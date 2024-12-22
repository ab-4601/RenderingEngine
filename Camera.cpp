#include "Camera.h"

Camera::Camera(glm::vec3 position, int windowWidth, int windowHeight, glm::vec3 worldUpDir,
	GLfloat pitch, GLfloat yaw, GLfloat moveSpeed, GLfloat turnSpeed) :
	position{ position }, worldUpDir{ worldUpDir }, pitch{ pitch }, yaw{ yaw },
	movementSpeed{ moveSpeed }, turnSpeed{ turnSpeed }, movementSpeedMultiplier{ 1.f } 
{

	glm::vec3 origin(0.f, 0.f, 0.f);
	this->front = glm::normalize(this->position - origin);

	float aspect = (float)windowWidth / windowHeight;
	this->projection = glm::perspective(glm::radians(45.f), aspect, ::near_plane, ::far_plane);

	glGenBuffers(1, &this->cameraBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, this->cameraBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4x4) * 2 + sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, this->cameraBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	update();
}

void Camera::keyFunctionality(const Window* currWindow, GLfloat deltaTime) {
	GLfloat velocity = this->movementSpeed * deltaTime * this->movementSpeedMultiplier;

	if (currWindow->getKeyPress(GLFW_KEY_W) || currWindow->getKeyPress(GLFW_KEY_UP))
		this->position += this->front * velocity;

	if (currWindow->getKeyPress(GLFW_KEY_S) || currWindow->getKeyPress(GLFW_KEY_DOWN))
		this->position -= this->front * velocity;

	if (currWindow->getKeyPress(GLFW_KEY_D) || currWindow->getKeyPress(GLFW_KEY_RIGHT))
		this->position += this->right * velocity;

	if (currWindow->getKeyPress(GLFW_KEY_A) || currWindow->getKeyPress(GLFW_KEY_LEFT))
		this->position -= this->right * velocity;

	if (currWindow->getKeyPress(GLFW_KEY_SPACE))
		this->position += this->up * velocity;

	if (currWindow->getKeyPress(GLFW_KEY_LEFT_CONTROL))
		this->position -= this->up * velocity;

	if (currWindow->getKeyPress(GLFW_KEY_LEFT_SHIFT))
		this->movementSpeedMultiplier = 3.f;
	else
		this->movementSpeedMultiplier = 1.f;
}

void Camera::mouseFunctionality(GLfloat xChange, GLfloat yChange, GLfloat scrollChange) {
	if (this->movementSpeed > 0)
		this->movementSpeed += scrollChange * 100;

	if (this->movementSpeed <= 0)
		this->movementSpeed = 1.f;

	xChange *= this->turnSpeed;
	yChange *= this->turnSpeed;

	this->yaw += xChange;
	this->pitch += yChange;

	if (this->pitch > 89.f) {
		this->pitch = 89.f;
	}


	if (this->pitch < -89.f) {
		this->pitch = -89.f;
	}

	this->update();
}

void Camera::update() {
	this->front.x = cosf(glm::radians(yaw)) * cosf(glm::radians(pitch));
	this->front.y = sinf(glm::radians(pitch));
	this->front.z = sinf(glm::radians(yaw)) * cosf(glm::radians(pitch));

	this->front = glm::normalize(this->front);

	this->right = glm::normalize(glm::cross(this->front, this->worldUpDir));
	this->up = glm::normalize(glm::cross(this->right, this->front));

	glm::mat4x4 cameraSpaceMatrices[2] = { this->projection, this->generateViewMatrix() };

	glBindBuffer(GL_UNIFORM_BUFFER, this->cameraBuffer);
	for (int i = 0; i < 3; i++) {
		if (i != 2)
			glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(glm::mat4x4), sizeof(glm::mat4x4), &cameraSpaceMatrices[i]);
		else
			glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(glm::mat4x4), sizeof(glm::vec3), &this->position);
	}
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}