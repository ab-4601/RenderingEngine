#pragma once

#include "PointLight.h"

class SpotLight : public PointLight
{
private:
	glm::vec3 direction;
	GLfloat edge, procEdge;

public:
	SpotLight(GLfloat aIntensity = 0.f, GLfloat dIntensity = 0.f, glm::vec3 position = { 0.f, 0.f, 0.f },
		GLfloat constant = 0.f, GLfloat linear = 0.f, GLfloat exponent = 0.f,
		glm::vec3 color = { 1.f, 1.f, 1.f }, glm::vec3 direction = { 0.f, 0.f, 0.f },
		GLfloat edge = 0.f);

	SpotLight(SpotLight&& source) noexcept;
	SpotLight(const SpotLight& source) noexcept;

	glm::vec3 getDirection() const { return this->direction; }
	GLfloat getEdge() const { return this->edge; }
	GLfloat getProcEdge() const { return this->procEdge; }

	void operator=(SpotLight&& rhs) noexcept;

	~SpotLight() = default;
};

