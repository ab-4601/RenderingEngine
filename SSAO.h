#pragma once

#include "Shader.h"
#include "Quad.h"

class SSAO {
private:
	GLuint FBO{ 0 }, blurFBO{ 0 }, colorBuffer{ 0 }, colorBufferBlur{ 0 }, noiseTexture{ 0 };

	std::vector<glm::vec3> kernel;
	std::vector<glm::vec3> noise;

	glm::vec2 scrResolution{ 0.f };

	Shader shader{ "ssao.vert", "ssao.frag" };
	Shader blurShader{ "ssao.vert", "ssaoBlur.frag" };

	Quad quad;

	float jitter(float x, float y) {
		static std::default_random_engine generator;
		static std::uniform_real_distribution<float> randomFloat(x, y);

		return randomFloat(generator);
	}

	void genTexture(GLuint& texID, GLenum colorAttachment, GLint internalFormat, GLenum format,
		int windowWidth, int windowHeight);

	void genNoiseTexture(int windowWidth, int windowHeight);

	float lerp(float a, float b, float c) {
		return a + c * (b - a);
	}

	bool checkFBOStatus() {
		GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if (status != GL_FRAMEBUFFER_COMPLETE)
			return false;

		return true;
	}

	void ssaoBlur();

public:
	SSAO(int windowWidth, int windowHeight);

	inline GLuint occlusionBuffer() const { return this->colorBufferBlur; }

	void _init(int windowWidth, int windowHeight);
	void calcSSAO(GLuint gPosition, GLuint gNormal, GLuint currFramebuffer = 0);

	~SSAO() {
		if (this->FBO != 0)
			glDeleteFramebuffers(1, &this->FBO);
	}
};