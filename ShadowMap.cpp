#include "ShadowMap.h"

ShadowMap::ShadowMap(float nearPlane, float farPlane) 
	: nearPlane{ nearPlane }, farPlane{ farPlane }
{
	this->projection = glm::perspective(
		glm::radians(90.f), (float)this->SHADOW_WIDTH / (float)this->SHADOW_HEIGHT, this->nearPlane, this->farPlane
	);

	this->_init();
}

void ShadowMap::_init() {
	glGenFramebuffers(1, &this->FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);

	glGenTextures(1, &this->depthMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->depthMap);

	for (int i = 0; i < 6; i++) {
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, 
			this->SHADOW_WIDTH, this->SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL
		);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, this->depthMap, 0);
	glReadBuffer(GL_NONE);
	glDrawBuffer(GL_NONE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMap::calculateShadowTransforms(glm::vec3 lightPosition) {
	this->shadowTransforms.clear();

	this->shadowTransforms.push_back(
		this->projection * glm::lookAt(lightPosition, lightPosition + glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, -1.f, 0.f))
	);

	this->shadowTransforms.push_back(
		this->projection * glm::lookAt(lightPosition, lightPosition + glm::vec3(-1.f, 0.f, 0.f), glm::vec3(0.f, -1.f, 0.f))
	);

	this->shadowTransforms.push_back(
		this->projection * glm::lookAt(lightPosition, lightPosition + glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 1.f))
	);

	this->shadowTransforms.push_back(
		this->projection * glm::lookAt(lightPosition, lightPosition + glm::vec3(0.f, -1.f, 0.f), glm::vec3(0.f, 0.f, -1.f))
	);

	this->shadowTransforms.push_back(
		this->projection * glm::lookAt(lightPosition, lightPosition + glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, -1.f, 0.f))
	);

	this->shadowTransforms.push_back(
		this->projection * glm::lookAt(lightPosition, lightPosition + glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, -1.f, 0.f))
	);
}

void ShadowMap::calculateShadowMap(std::vector<Mesh*>& meshes, int windowWidth, int windowHeight, 
	glm::vec3 lightPosition, GLuint currentFramebuffer) 
{
	glViewport(0, 0, this->SHADOW_WIDTH, this->SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	glCullFace(GL_FRONT);

	this->shader.useShader();

	this->shader.setFloat("farPlane", this->farPlane);
	this->shader.setVec3("lightPosition", lightPosition);

	std::string buffer{};

	for (int i = 0; i < 6; i++) {
		buffer = "shadowMatrices[" + std::to_string(i) + "]";
		this->shader.setMat4(buffer.data(), this->shadowTransforms[i]);
	}

	for (int i = 0; i < meshes.size(); i++) {
		this->shader.setMat4("model", meshes[i]->getModelMatrix());

		meshes[i]->drawMesh(GL_TRIANGLES);
	}

	glViewport(0, 0, windowWidth, windowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, currentFramebuffer);

	glCullFace(GL_BACK);
}

ShadowMap::~ShadowMap() {
	if (this->FBO != 0)
		glDeleteFramebuffers(1, &this->FBO);

	if (this->depthMap != 0)
		glDeleteTextures(1, &this->depthMap);
}