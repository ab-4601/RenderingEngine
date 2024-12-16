#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(GLfloat aIntensity,
	GLfloat dIntensity, glm::vec3 direction, glm::vec3 color)
	: Light(aIntensity, dIntensity, color), direction{ direction } {

}

void DirectionalLight::useLight(GLuint ambientIntensityLocation, GLuint ambientColorLocation,
	GLuint diffuseIntensityLocation, GLuint directionLocation) {

	Light::useLight(ambientIntensityLocation, ambientColorLocation, diffuseIntensityLocation);

	glUniform3f(directionLocation, this->direction.x, this->direction.y, this->direction.z);
}