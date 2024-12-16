#include "PointLight.h"

PointLight::PointLight(GLfloat aIntensity, GLfloat dIntensity,
	glm::vec3 position, GLfloat constant, GLfloat linear, GLfloat exponent, glm::vec3 color)
	: Light(aIntensity, dIntensity, color), position{ position },
	constant{ constant }, linear{ linear }, exponent{ exponent } {

}

PointLight::PointLight(PointLight&& source) noexcept
	: Light(source.ambientIntensity, source.diffuseIntensity, source.color), position{ source.position },
	constant{ source.constant }, linear{ source.linear }, exponent{ source.exponent } {

	source.~PointLight();
}

PointLight::PointLight(const PointLight& source) noexcept
	: Light(source.ambientIntensity, source.diffuseIntensity, source.color), position{ source.position },
	constant{ source.constant }, linear{ source.linear }, exponent{ source.exponent } {

}

void PointLight::operator=(PointLight&& rhs) noexcept {
	this->ambientIntensity = rhs.ambientIntensity;
	this->diffuseIntensity = rhs.diffuseIntensity;
	this->color = rhs.color;
	this->position = rhs.position;
	this->constant = rhs.constant;
	this->linear = rhs.linear;
	this->exponent = rhs.exponent;
}

void PointLight::useLight(GLuint ambientIntensityLocation, GLuint ambientColorLocation, GLuint diffuseIntensityLocation,
	GLuint positionLocation, GLuint constantLocation, GLuint linearLocation, GLuint exponentLocation) {

	Light::useLight(ambientIntensityLocation, ambientColorLocation, diffuseIntensityLocation);

	glUniform3f(positionLocation, this->position.x, this->position.y, this->position.z);
	glUniform1f(constantLocation, this->constant);
	glUniform1f(linearLocation, this->linear);
	glUniform1f(exponentLocation, this->exponent);
}