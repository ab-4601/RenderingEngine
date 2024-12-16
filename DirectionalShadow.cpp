#include "DirectionalShadow.h"

DirectionalShadow::DirectionalShadow(float viewFrustumSize, float nearPlane, float farPlane) {
	this->projection = glm::ortho(
		-viewFrustumSize, viewFrustumSize, -viewFrustumSize, viewFrustumSize, nearPlane, farPlane
	);
}

void DirectionalShadow::checkFramebufferStatus(const char* errorMessage) {
	GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << errorMessage << std::endl;
		exit(-1);
	}
}

void DirectionalShadow::_init() {
	glGenFramebuffers(1, &this->FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);

	glGenTextures(1, &this->depthMap);
	glBindTexture(GL_TEXTURE_2D, this->depthMap);

	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, this->SHADOW_WIDTH, this->SHADOW_HEIGHT,
		0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL
	);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	float borderColor[] = { 1.f, 1.f, 1.f, 1.f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->depthMap, 0);
	glReadBuffer(GL_NONE);
	glDrawBuffer(GL_NONE);

	this->checkFramebufferStatus("Shadow framebuffer initialization failed");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void DirectionalShadow::calculateShadows(int windowWidth, int windowHeight, 
	std::vector<Mesh*>& meshes, glm::vec3 lightPosition, GLuint currentFramebuffer) 
{
	glViewport(0, 0, this->SHADOW_WIDTH, this->SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	this->shader.useShader();

	this->view = glm::lookAt(
		lightPosition,
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(0.f, 1.f, 0.f)
	);

	this->shader.setMat4("projection", this->projection);
	this->shader.setMat4("view", this->view);

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(3.f, 3.f);

	for (size_t i = 0; i < meshes.size(); i++) {
		this->shader.setMat4("model", meshes[i]->getModelMatrix());
		meshes[i]->drawMesh(GL_TRIANGLES);
	}

	glDisable(GL_POLYGON_OFFSET_FILL);

	glBindFramebuffer(GL_FRAMEBUFFER, currentFramebuffer);
	glViewport(0, 0, windowWidth, windowHeight);

	this->shader.endShader();
}

DirectionalShadow::~DirectionalShadow() {
	if (this->FBO != 0)
		glDeleteFramebuffers(1, &this->FBO);

	if (this->depthMap != 0)
		glDeleteTextures(1, &this->depthMap);
}