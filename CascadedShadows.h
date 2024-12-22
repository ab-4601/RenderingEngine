#pragma once

#include "Model.h"

class CascadedShadows {
private:
	static const int mapResolution = 2048;
	static const uint numCascades = 4;

	GLuint FBO{ 0 }, SSBO{ 0 };
	GLuint shadowMaps{ 0 }, randomOffset{ 0 };

	float cascadeSplits[::MAX_CASCADES];
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
	void genRandomOffsetData(int size, int samplesU, int samplesV);
	void setComputeUniforms();

public:
	CascadedShadows(int windowWidth = 800, int windowHeight = 600, float lambda = 0.5f,
		int size = 10, int samplesU = 4, int samplesV = 4);

	void _init();

	inline GLuint getNumCascades() const { return this->numCascades; }
	inline GLuint getShadowMaps() const { return this->shadowMaps; }
	inline GLuint noiseBuffer() const { return this->randomOffset; }
	inline const float* const cascadePlanes() const { return &this->cascadeSplits[0]; }
	inline glm::vec3 getNoiseTextureSize() const { return this->noiseTextureSize; }

	void calculateShadows(int windowWidth, int windowHeight, std::vector<Mesh*>& meshes, glm::vec3 lightPosition,
		GLuint currFramebuffer = 0);

	~CascadedShadows();
};