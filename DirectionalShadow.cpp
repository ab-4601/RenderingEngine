#include "DirectionalShadow.h"

DirectionalShadow::DirectionalShadow(float viewFrustumSize, float nearPlane, float farPlane) {
	projection = glm::ortho(
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
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);

	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT,
		0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL
	);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	float borderColor[] = { 1.f, 1.f, 1.f, 1.f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glReadBuffer(GL_NONE);
	glDrawBuffer(GL_NONE);

	checkFramebufferStatus("Shadow framebuffer initialization failed");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void DirectionalShadow::calculateShadows(int windowWidth, int windowHeight, 
	std::vector<Mesh*>& meshes, glm::vec3 lightPosition, GLuint currentFramebuffer) 
{
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	shader.useShader();

	view = glm::lookAt(
		lightPosition,
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(0.f, 1.f, 0.f)
	);

	shader.setMat4("projection", projection);
	shader.setMat4("view", view);

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(3.f, 3.f);

	for (size_t i = 0; i < meshes.size(); i++) {
		shader.setMat4("model", meshes[i]->getModelMatrix());
		meshes[i]->drawMesh(GL_TRIANGLES);
	}

	glDisable(GL_POLYGON_OFFSET_FILL);

	glBindFramebuffer(GL_FRAMEBUFFER, currentFramebuffer);
	glViewport(0, 0, windowWidth, windowHeight);

	shader.endShader();
}

DirectionalShadow::~DirectionalShadow() {
	if (FBO != 0)
		glDeleteFramebuffers(1, &FBO);

	if (depthMap != 0)
		glDeleteTextures(1, &depthMap);
}