#include "Light.h"

Light::Light(GLfloat aIntensity, GLfloat dIntensity, glm::vec3 color)
	: color{ color }, ambientIntensity{ aIntensity },
	diffuseIntensity{ dIntensity } {

}