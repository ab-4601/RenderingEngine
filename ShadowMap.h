#pragma once

#include "Core.h"
#include "Mesh.h"
#include "Shader.h"

class ShadowMap {
private:
	static const int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

	GLuint FBO = 0;
	GLuint depthMap = 0;

	float nearPlane = 1.f, farPlane = 25.f;

	glm::mat4 projection{ 1.f };
	std::vector<glm::mat4> shadowTransforms{};

	Shader shader{ "shadow.vert", "shadow.frag", "shadow.geom" };

public:
	ShadowMap(float nearPlane = 1.f, float farPlane = 25.f);

	void _init();
	void calculateShadowTransforms(glm::vec3 lightPosition);
	void calculateShadowMap(std::vector<Mesh*>& meshes, int windowWidth, int windowHeight,
		glm::vec3 lightPosition, GLuint currentFramebuffer = 0);

	inline GLuint getShadowCubemapID() const { return this->depthMap; }

	~ShadowMap();
};