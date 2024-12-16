#include "LightSources.h"

LightSources::LightSources() {
	this->sourceMesh.createMesh();
}

void LightSources::renderLightSources(DirectionalLight& directionalLight,
	std::vector<PointLight>& pointLights, std::vector<SpotLight>& spotLights, int pointLightCount, int spotLightCount) 
{
	this->shader.useShader();

	glm::mat4 model = glm::mat4(1.f);

	this->shader.setFloat("intensity", 50.f);
	this->shader.setVec3("lightColor", directionalLight.getLightColor());

	glm::vec3 dirLightDirection = directionalLight.getLightDirection();

	model = glm::translate(model, dirLightDirection);
	model = glm::scale(model, glm::vec3(100, 100, 100));
	this->shader.setMat4("model", model);

	this->sourceMesh.renderMesh();

	for (int i = 0; i < pointLightCount; i++) {
		model = glm::mat4(1.f);
		this->shader.setVec3("lightColor", pointLights[i].getLightColor());
		
		model = glm::translate(model, pointLights[i].getPosition());
		this->shader.setMat4("model", model);
		this->sourceMesh.renderMesh();
	}

	for (int i = 0; i < spotLightCount; i++) {
		model = glm::mat4(1.f);
		this->shader.setVec3("lightColor", spotLights[i].getLightColor());

		model = glm::translate(model, spotLights[i].getPosition());
		this->shader.setMat4("model", model);
		this->sourceMesh.renderMesh();
	}

	this->shader.endShader();
}