#include "LightSources.h"

LightSources::LightSources() {
	sourceMesh.createMesh();
}

void LightSources::renderLightSources(DirectionalLight& directionalLight,
	std::vector<PointLight>& pointLights, std::vector<SpotLight>& spotLights, int pointLightCount, int spotLightCount) 
{
	shader.useShader();

	glm::mat4 model = glm::mat4(1.f);

	shader.setFloat("intensity", 50.f);
	shader.setVec3("lightColor", directionalLight.getLightColor());

	glm::vec3 dirLightDirection = directionalLight.getLightDirection();

	model = glm::translate(model, dirLightDirection);
	model = glm::scale(model, glm::vec3(100, 100, 100));
	shader.setMat4("model", model);

	sourceMesh.renderMesh();

	for (int i = 0; i < pointLightCount; i++) {
		model = glm::mat4(1.f);
		shader.setVec3("lightColor", pointLights[i].getLightColor());
		
		model = glm::translate(model, pointLights[i].getPosition());
		shader.setMat4("model", model);
		sourceMesh.renderMesh();
	}

	for (int i = 0; i < spotLightCount; i++) {
		model = glm::mat4(1.f);
		shader.setVec3("lightColor", spotLights[i].getLightColor());

		model = glm::translate(model, spotLights[i].getPosition());
		shader.setMat4("model", model);
		sourceMesh.renderMesh();
	}

	shader.endShader();
}