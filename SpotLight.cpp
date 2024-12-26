#include "SpotLight.h"

SpotLight::SpotLight(GLfloat aIntensity, GLfloat dIntensity, glm::vec3 position,
	GLfloat constant, GLfloat linear, GLfloat exponent,
	glm::vec3 color, glm::vec3 direction, GLfloat edge)
	: PointLight(aIntensity, dIntensity, position, constant, linear, exponent, color),
	direction{ direction }, edge{ edge }, procEdge{ cosf(glm::radians(edge)) } {

}

SpotLight::SpotLight(SpotLight&& source) noexcept
	: PointLight(source.ambientIntensity, source.diffuseIntensity, source.position, source.constant,
		source.linear, source.exponent, source.color), direction{ source.direction }, edge{ source.edge },
	procEdge{ source.procEdge } {

	source.~SpotLight();
}

SpotLight::SpotLight(const SpotLight& source) noexcept
	: PointLight(source.ambientIntensity, source.diffuseIntensity, source.position, source.constant,
		source.linear, source.exponent, source.color), direction{ source.direction }, edge{ source.edge },
	procEdge{ source.procEdge } {

}

void SpotLight::operator=(SpotLight&& rhs) noexcept {
	this->ambientIntensity = rhs.ambientIntensity;
	this->diffuseIntensity = rhs.diffuseIntensity;
	this->color = rhs.color;
	this->position = rhs.position;
	this->direction = rhs.direction;
	this->constant = rhs.constant;
	this->linear = rhs.linear;
	this->exponent = rhs.exponent;
	this->edge = rhs.edge;
	this->procEdge = rhs.procEdge;
}