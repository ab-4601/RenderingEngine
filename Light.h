#pragma once

#include "Core.h"

class Light {
protected:
	glm::vec3 color;

	GLfloat ambientIntensity;
	GLfloat diffuseIntensity;

public:
	Light(GLfloat aIntensity = 0.f, GLfloat dIntensity = 0.f, glm::vec3 color = { 1.f, 1.f, 1.f });

	virtual glm::vec3 getLightColor() {
		return this->color;
	}

	void useLight(GLuint ambientIntensityLocation, GLuint ambientColorLocation, GLuint diffuseIntensityLocation);

	~Light() = default;
};
