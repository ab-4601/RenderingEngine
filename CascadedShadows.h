#pragma once

#include "Core.h"
#include "Mesh.h"
#include "Shader.h"

class CascadedShadows {
private:
	static const int mapResolution = 2048;
	static const uint numCascades = 4;

	GLuint FBO{ 0 }, UBO{ 0 };
	GLuint shadowMaps{ 0 }, randomOffset{ 0 };

	std::vector<float> cascadeSplits;
	glm::mat4 lightSpaceMatrices[::MAX_CASCADES];
	glm::vec4 frustumCorners[8];
	glm::vec3 noiseTextureSize{ 0.f };

	Shader shader{ "CSM.vert", "CSM.frag", "CSM.geom" };
	Shader computeShader{ "CSM.comp" };

	float jitter(float x, float y) {
		static std::default_random_engine generator;
		static std::uniform_real_distribution<float> randomFloat(x, y);

		return randomFloat(generator);
	}

	bool checkFramebufferStatus();
	void calcSplitDepths(float lambda);
	void calcFrustumCorners(glm::mat4 projection, glm::mat4 view);
	void genRandomOffsetData(int size, int samplesU, int samplesV);

	glm::mat4 calcLightSpaceMatrix(glm::mat4 view, glm::vec3 lightDir, int windowWidth, int windowHeight,
		const float& nearPlane, const float& farPlane);

	void calcLightSpaceMatrices(glm::mat4 view, glm::vec3 lightDir, int windowWidth, int windowHeight);

public:
	CascadedShadows(float lambda = 0.5f, int size = 10, int samplesU = 4, int samplesV = 4);

	void _init();

	inline GLuint getNumCascades() const { return this->numCascades; }
	inline GLuint getShadowMaps() const { return this->shadowMaps; }
	inline GLuint noiseBuffer() const { return this->randomOffset; }
	inline std::vector<float> cascadePlanes() const { return this->cascadeSplits; }
	inline glm::vec3 getNoiseTextureSize() const { return this->noiseTextureSize; }

	void calculateShadows(glm::mat4 view, int windowWidth, int windowHeight, std::vector<Mesh*>& meshes,
		glm::vec3 lightPosition, GLuint currFramebuffer = 0);

	~CascadedShadows();
};