#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(GLfloat aIntensity,
	GLfloat dIntensity, glm::vec3 direction, glm::vec3 color)
	: Light(aIntensity, dIntensity, color), direction{ direction } {

}