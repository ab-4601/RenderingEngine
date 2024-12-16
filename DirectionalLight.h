#pragma once

#include "Light.h"

class DirectionalLight : public Light {
private:
	glm::vec3 direction;

public:
	DirectionalLight(GLfloat aIntensity = 0.f, GLfloat dIntensity = 0.f,
		glm::vec3 direction = { 0.f, 0.f, 0.f }, glm::vec3 color = { 1.f, 1.f, 1.f });

	inline void updateLightLocation(glm::vec3 pos) {
		this->direction = pos;
	}

	inline glm::vec3 getLightDirection() const {
		return this->direction;
	}

	void useLight(GLuint ambientIntensityLocation, GLuint ambientColorLocation, GLuint diffuseIntensityLocation,
		GLuint directionLocation);

	~DirectionalLight() = default;
};

