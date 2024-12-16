#include "Light.h"

Light::Light(GLfloat aIntensity, GLfloat dIntensity, glm::vec3 color)
	: color{ color }, ambientIntensity{ aIntensity },
	diffuseIntensity{ dIntensity } {

}

void Light::useLight(GLuint ambientIntensityLocation, GLuint ambientColorLocation, GLuint diffuseIntensityLocation) {
	glUniform3f(ambientColorLocation, this->color.x, this->color.y, this->color.z);
	glUniform1f(ambientIntensityLocation, this->ambientIntensity);
	glUniform1f(diffuseIntensityLocation, this->diffuseIntensity);
}