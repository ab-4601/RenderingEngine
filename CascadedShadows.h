#pragma once

#include "Core.h"
#include "Mesh.h"
#include "Shader.h"

class CascadedShadows {
private:
	static const int mapResolution = 2046;
	static const uint numCascades = 4;

	GLuint FBO{ 0 }, UBO{ 0 };
	GLuint shadowMaps{ 0 }, randomOffset{ 0 };

	int offsetTextureSamplesU = 4, offsetTextureSamplesV = 4, offsetTextureSize = 10;

	std::vector<float> cascadeSplits;
	std::vector<glm::mat4> lightSpaceMatrices;
	std::vector<glm::vec4> frustumCorners;

	Shader shader{ "CSM.vert", "CSM.frag", "CSM.geom" };

	float jitter(float x, float y) {
		static std::default_random_engine generator;
		static std::uniform_real_distribution<float> randomFloat(x, y);

		return randomFloat(generator);
	}

	bool checkFramebufferStatus();
	void calcSplitDepths(float lambda);
	void calcFrustumCorners(glm::mat4 projection, glm::mat4 view);
	void genRandomOffsetData();

	void calcLightSpaceMatrix(glm::mat4 view, glm::vec3 lightDir, int windowWidth, int windowHeight,
		const float& nearPlane, const float& farPlane);

	void calcLightSpaceMatrices(glm::mat4 view, glm::vec3 lightDir, int windowWidth, int windowHeight);

public:
	CascadedShadows(float lambda = 0.5f, int size = 4, int samplesU = 4, int samplesV = 4);

	void _init();

	inline GLuint getNumCascades() const { return this->numCascades; }
	inline GLuint getShadowMaps() const { return this->shadowMaps; }
	inline GLuint noiseBuffer() const { return this->randomOffset; }
	inline std::vector<float> cascadePlanes() const { return this->cascadeSplits; }
	glm::vec3 getNoiseTextureSize() const {
		float x = (float)this->offsetTextureSize;
		float y = (float)this->offsetTextureSize;
		float z = (float)this->offsetTextureSamplesU * (float)this->offsetTextureSamplesV / 2.f;

		return glm::vec3(x, y, z);
	}

	void calculateShadows(glm::mat4 view, int windowWidth, int windowHeight, std::vector<Mesh*>& meshes,
		glm::vec3 lightPosition, GLuint currFramebuffer = 0);

	~CascadedShadows();
};