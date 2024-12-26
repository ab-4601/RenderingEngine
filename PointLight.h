#pragma once

#include "Core.h"
#include "Light.h"

class PointLight : public Light {
protected:
	glm::vec3 position;
	GLfloat constant, linear, exponent;

public:
	PointLight(GLfloat aIntensity = 0.f, GLfloat dIntensity = 0.f, glm::vec3 position = { 0.f, 0.f, 0.f },
		GLfloat constant = 0.f, GLfloat linear = 0.f, GLfloat exponent = 0.f, glm::vec3 color = { 1.f, 1.f, 1.f });

	PointLight(PointLight&& source) noexcept;
	PointLight(const PointLight& source) noexcept;

	glm::vec3 getPosition() const { return this->position; }
	GLfloat getAttenuationConstant() const { return this->constant; }
	GLfloat getAttenuationLinear() const { return this->linear; }
	GLfloat getAttenuationExponent() const { return this->exponent; }

	virtual void updateLightLocation(glm::vec3 pos) {
		this->position = pos;
	}

	void operator=(PointLight&& rhs) noexcept;

	~PointLight() = default;
};
