#pragma once

#include "Core.h"
#include "Mesh.h"
#include "Shader.h"

class DirectionalShadow {
private:
	static const int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

	GLuint FBO = 0;
	GLuint depthMap = 0;

	float nearPlane = 1.f;
	float farPlane = 25.f;

	glm::mat4 projection = glm::mat4(1.f);
	glm::mat4 view = glm::mat4(1.f);

	Shader shader{ "directionalShadow.vert", "directionalShadow.frag" };

	void checkFramebufferStatus(const char* errorMessage = nullptr);

public:
	DirectionalShadow(float viewFrustumSize = 10.f, float nearPlane = 1.f, float farPlane = 25.f);

	void _init();

	void calculateShadows(int windowWidth, int windowHeight, std::vector<Mesh*>& meshes, 
		glm::vec3 lightPosition, GLuint currentFramebuffer = 0);

	glm::mat4 getLightSpaceMatrix() const { return this->projection * this->view; }
	GLuint getDirectionalShadowMap() const { return this->depthMap; }

	~DirectionalShadow();
};

