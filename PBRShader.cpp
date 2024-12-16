#include "PBRShader.h"

PBRShader::PBRShader(std::string vertFileName, std::string fragFileName, std::string geomFileName)
	: IShaderLoader(vertFileName, fragFileName, geomFileName)
{
	for (int i = 0; i < ::MAX_CASCADES; i++) {
		this->uniformCascadePlaneDistances[i] = 0;
	}

	this->readFile(this->vertFileName, this->vertexShader);
	this->readFile(this->fragFileName, this->fragmentShader);
	this->readFile(this->geomFileName, this->geometryShader);

	this->compileShader(this->vertexShaderID, this->vertexShader, GL_VERTEX_SHADER);
	this->compileShader(this->fragmentShaderID, this->fragmentShader, GL_FRAGMENT_SHADER);
	this->compileShader(this->geometryShaderID, this->geometryShader, GL_GEOMETRY_SHADER);

	this->attachShader({ vertexShaderID, fragmentShaderID, geometryShaderID });

	glDeleteShader(this->geometryShaderID);
	glDeleteShader(this->vertexShaderID);
	glDeleteShader(this->fragmentShaderID);
}

void PBRShader::getUniformLocations() {
	this->uniformModel = glGetUniformLocation(this->programID, "model");

	this->uniformColor = glGetUniformLocation(this->programID, "color");

	this->uniformDirectionalLight.uniformAmbientColor = glGetUniformLocation(this->programID,
		"directionalLight.base.color");
	this->uniformDirectionalLight.uniformAmbientIntensity = glGetUniformLocation(this->programID,
		"directionalLight.base.ambientIntensity");
	this->uniformDirectionalLight.uniformDiffuseIntensity = glGetUniformLocation(this->programID,
		"directionalLight.base.diffuseIntensity");
	this->uniformDirectionalLight.uniformDirection = glGetUniformLocation(this->programID, "directionalLight.direction");

	this->uniformAlbedo = glGetUniformLocation(this->programID, "material.albedo");
	this->uniformMetallic = glGetUniformLocation(this->programID, "material.metallic");
	this->uniformRoughness = glGetUniformLocation(this->programID, "material.roughness");
	this->uniformAo = glGetUniformLocation(this->programID, "material.ao");

	this->uniformCameraPosition = glGetUniformLocation(this->programID, "cameraPos");

	this->uniformTextureBool = glGetUniformLocation(this->programID, "useDiffuseMap");
	this->uniformUseNormalMap = glGetUniformLocation(this->programID, "useNormalMap");
	this->uniformStrippedNormalBool = glGetUniformLocation(this->programID, "strippedNormalMap");
	this->uniformUseMaterialMap = glGetUniformLocation(this->programID, "useMaterialMap");

	this->uniformDiffuseSampler = glGetUniformLocation(this->programID, "diffuseMap");
	this->uniformNormalSampler = glGetUniformLocation(this->programID, "normalMap");
	this->uniformDepthSampler = glGetUniformLocation(this->programID, "depthMap");
	this->uniformMetallicSampler = glGetUniformLocation(this->programID, "metallicMap");
	this->uniformRoughnessSampler = glGetUniformLocation(this->programID, "roughnessMap");
	this->uniformIrradianceSampler = glGetUniformLocation(this->programID, "irradianceMap");
	this->uniformBRDFSampler = glGetUniformLocation(this->programID, "brdfLUT");
	this->uniformPrefilterSampler = glGetUniformLocation(this->programID, "prefilterMap");
	this->uniformRandomOffsetSampler = glGetUniformLocation(this->programID, "randomOffsets");
	this->uniformSSAOSampler = glGetUniformLocation(this->programID, "ssao");

	this->uniformCSMSampler = glGetUniformLocation(this->programID, "cascadedShadowMap");
	this->uniformCascadeCount = glGetUniformLocation(this->programID, "cascadeCount");
	this->uniformPointShadowSampler = glGetUniformLocation(this->programID, "pointShadowMap");
	this->uniformLightSpaceTransform = glGetUniformLocation(this->programID, "lightSpaceTransform");
	this->uniformCalcShadows = glGetUniformLocation(this->programID, "calcShadows");
	this->uniformSSAObool = glGetUniformLocation(this->programID, "enableSSAO");
	this->uniformNearPlane = glGetUniformLocation(this->programID, "nearPlane");
	this->uniformFarPlane = glGetUniformLocation(this->programID, "farPlane");
	this->uniformRadius = glGetUniformLocation(this->programID, "radius");
	this->uniformOffsetTextureSize = glGetUniformLocation(this->programID, "offsetTexSize");

	this->uniformPointLightCount = glGetUniformLocation(this->programID, "pointLightCount");
	this->uniformSpotLightCount = glGetUniformLocation(this->programID, "spotLightCount");

	std::string buffer{};

	for (uint i = 0; i < ::MAX_CASCADES; i++) {
		buffer = "cascadePlanes[" + std::to_string(i) + "]";
		this->uniformCascadePlaneDistances[i] = glGetUniformLocation(this->programID, buffer.data());
	}

	buffer.clear();

	for (int i = 0; i < (int)::MAX_POINT_LIGHTS; i++) {
		buffer = "pointLights[" + std::to_string(i) + "].base.color";
		this->uniformPointLight[i].uniformColor = glGetUniformLocation(this->programID, buffer.data());

		buffer = "pointLights[" + std::to_string(i) + "].position";
		this->uniformPointLight[i].uniformPosition = glGetUniformLocation(this->programID, buffer.data());

		buffer = "pointLights[" + std::to_string(i) + "].constant";
		this->uniformPointLight[i].uniformConstant = glGetUniformLocation(this->programID, buffer.data());

		buffer = "pointLights[" + std::to_string(i) + "].linear";
		this->uniformPointLight[i].uniformLinear = glGetUniformLocation(this->programID, buffer.data());

		buffer = "pointLights[" + std::to_string(i) + "].exponent";
		this->uniformPointLight[i].uniformExponent = glGetUniformLocation(this->programID, buffer.data());
	}

	buffer.clear();

	for (int i = 0; i < (int)::MAX_SPOT_LIGHTS; i++) {
		buffer = "spotLights[" + std::to_string(i) + "].base.base.color";
		this->uniformSpotLight[i].uniformColor = glGetUniformLocation(this->programID, buffer.data());

		buffer = "spotLights[" + std::to_string(i) + "].base.position";
		this->uniformSpotLight[i].uniformPosition = glGetUniformLocation(this->programID, buffer.data());

		buffer = "spotLights[" + std::to_string(i) + "].base.constant";
		this->uniformSpotLight[i].uniformConstant = glGetUniformLocation(this->programID, buffer.data());

		buffer = "spotLights[" + std::to_string(i) + "].base.linear";
		this->uniformSpotLight[i].uniformLinear = glGetUniformLocation(this->programID, buffer.data());

		buffer = "spotLights[" + std::to_string(i) + "].base.exponent";
		this->uniformSpotLight[i].uniformExponent = glGetUniformLocation(this->programID, buffer.data());

		buffer = "spotLights[" + std::to_string(i) + "].direction";
		this->uniformSpotLight[i].uniformDirection = glGetUniformLocation(this->programID, buffer.data());

		buffer = "spotLights[" + std::to_string(i) + "].edge";
		this->uniformSpotLight[i].uniformEdge = glGetUniformLocation(this->programID, buffer.data());
	}
}

void PBRShader::setDirectionalLight(DirectionalLight* directionalLight) {
	directionalLight->useLight(this->uniformDirectionalLight.uniformAmbientIntensity,
		this->uniformDirectionalLight.uniformAmbientColor, this->uniformDirectionalLight.uniformDiffuseIntensity,
		this->uniformDirectionalLight.uniformDirection);
}

void PBRShader::setPointLights(PointLight* pointLights, unsigned int lightCount) {
	if (lightCount > MAX_POINT_LIGHTS)
		lightCount = MAX_POINT_LIGHTS;

	glUniform1i(this->uniformPointLightCount, lightCount);

	for (size_t i = 0; i < lightCount; i++) {
		pointLights[i].useLight(this->uniformPointLight[i].uniformAmbientIntensity,
			this->uniformPointLight[i].uniformColor, this->uniformPointLight[i].uniformDiffuseIntensity,
			this->uniformPointLight[i].uniformPosition, this->uniformPointLight[i].uniformConstant,
			this->uniformPointLight[i].uniformLinear, this->uniformPointLight[i].uniformExponent);
	}
}

void PBRShader::setSpotLights(SpotLight* spotLights, unsigned int lightCount) {
	if (lightCount > MAX_SPOT_LIGHTS)
		lightCount = MAX_POINT_LIGHTS;

	glUniform1i(this->uniformSpotLightCount, lightCount);

	for (size_t i = 0; i < lightCount; i++) {
		spotLights[i].useLight(this->uniformSpotLight[i].uniformAmbientIntensity,
			this->uniformSpotLight[i].uniformColor, this->uniformSpotLight[i].uniformDiffuseIntensity,
			this->uniformSpotLight[i].uniformPosition, this->uniformSpotLight[i].uniformConstant,
			this->uniformSpotLight[i].uniformLinear, this->uniformSpotLight[i].uniformExponent,
			this->uniformSpotLight[i].uniformDirection, this->uniformSpotLight[i].uniformEdge);
	}
}

void PBRShader::setGeneralUniforms(DirectionalLight& directionalLight, std::vector<PointLight>& pointLights,
	int pointLightCount, std::vector<SpotLight>& spotLights, int spotLightCount, int numCascades,
	std::vector<float> cascadePlanes, float shadowRadius, glm::vec3 offsetTextureSize, GLuint irradianceMap,
	GLuint brdfSampler, GLuint prefilterSampler, GLuint noiseSampler, GLuint cascadedShadowMap, GLuint pointShadowMap)
{
	glUseProgram(this->programID);

	this->setDirectionalLight(&directionalLight);
	this->setPointLights(pointLights.data(), pointLightCount);
	this->setSpotLights(spotLights.data(), spotLightCount);

	glUniform1f(this->uniformFarPlane, ::far_plane);
	glUniform1f(this->uniformNearPlane, ::near_plane);
	glUniform1f(this->uniformRadius, shadowRadius);
	glUniform3fv(this->uniformOffsetTextureSize, 1, glm::value_ptr(offsetTextureSize));
	glUniform1i(this->uniformCascadeCount, numCascades);

	for (size_t i = 0; i < cascadePlanes.size(); i++)
		glUniform1f(this->uniformCascadePlaneDistances[i], cascadePlanes[i]);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterSampler);

	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, brdfSampler);

	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_CUBE_MAP, pointShadowMap);

	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_2D_ARRAY, cascadedShadowMap);

	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_3D, noiseSampler);
}