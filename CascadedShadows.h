#pragma once

#include "Model.h"

class CascadedShadows {
private:
	static const int mapResolution = 1024;
	static const uint numCascades = 4;

	GLuint FBO{ 0 }, SSBO{ 0 };
	GLuint shadowMaps{ 0 }, randomOffset{ 0 };

	float cascadeSplits[::MAX_CASCADES];
	glm::mat4 lightSpaceMatrices[::MAX_CASCADES];
	glm::vec4 frustumCorners[8];
	glm::vec3 noiseTextureSize{ 0.f };

	float aspect{ 0.f };

	Shader shader{ "CSM.vert", "CSM.frag", "CSM.geom" };
	Shader computeShader{ "CSM.comp" };

	float jitter(float x, float y) {
		static std::default_random_engine generator;
		static std::uniform_real_distribution<float> randomFloat(x, y);

		return randomFloat(generator);
	}

	bool checkFramebufferStatus();
	void calcSplitDepths(float lambda);
	void calcFrustumCorners(const glm::mat4& projection, const glm::mat4& view);
	
	glm::mat4 calcLightSpaceMatrix(const glm::mat4& view, const glm::vec3& lightDir, int windowWidth, int windowHeight,
		const float& nearPlane, const float& farPlane);

	void calcLightSpaceMatrices(const glm::mat4& view, const glm::vec3& lightDir, int windowWidth, int windowHeight);

	void genRandomOffsetData(int size, int samplesU, int samplesV);
	void setComputeUniforms();

public:
	CascadedShadows(int windowWidth = 800, int windowHeight = 600, float lambda = 0.5f,
		int size = 10, int samplesU = 4, int samplesV = 4);

	void _init();

	GLuint getNumCascades() const { return this->numCascades; }
	GLuint getShadowMaps() const { return this->shadowMaps; }
	GLuint noiseBuffer() const { return this->randomOffset; }
	const float* const cascadePlanes() const { return &this->cascadeSplits[0]; }
	glm::vec3 getNoiseTextureSize() const { return this->noiseTextureSize; }

	void calculateShadows(int windowWidth, int windowHeight, const std::vector<Mesh*>& meshes,
		const std::vector<Model*>& models, const glm::vec3& lightPosition, GLuint currFramebuffer = 0);

	~CascadedShadows();
};