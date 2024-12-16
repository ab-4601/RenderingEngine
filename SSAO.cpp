#include "SSAO.h"

SSAO::SSAO(int windowWidth, int windowHeight) {
	this->kernel.clear();
	this->noise.clear();

	this->quad.createQuad();
	this->scrResolution = glm::vec2(windowWidth, windowHeight);

	// sample kernel generation
	for (uint i = 0; i < 64; i++) {
		glm::vec3 sample(
			this->jitter(0.f, 1.f) * 2.f - 1.f,
			this->jitter(0.f, 1.f) * 2.f - 1.f,
			this->jitter(0.f, 1.f)
		);

		if (i < 16) {
			glm::vec3 noise{
				this->jitter(0.f, 1.f) * 2.f - 1.f,
				this->jitter(0.f, 1.f) * 2.f - 1.f,
				0.f
			};

			this->noise.push_back(noise);
		}

		sample = glm::normalize(sample);
		sample *= this->jitter(0.f, 1.f);
		float scale = (float)i / 64.f;

		scale = this->lerp(0.1f, 1.f, scale * scale);
		sample *= scale;

		this->kernel.push_back(sample);
	}

	this->_init(windowWidth, windowHeight);
	this->genNoiseTexture(windowWidth, windowHeight);
}

void SSAO::genTexture(GLuint& texID, GLenum colorAttachment, GLint internalFormat, GLenum format,
	int windowWidth, int windowHeight) 
{
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, windowWidth, windowHeight, 0, format, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, colorAttachment, GL_TEXTURE_2D, texID, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void SSAO::genNoiseTexture(int windowWidth, int windowHeight) {
	glGenTextures(1, &this->noiseTexture);
	glBindTexture(GL_TEXTURE_2D, this->noiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGBA, GL_FLOAT, this->noise.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void SSAO::_init(int windowWidth, int windowHeight) {
	glGenFramebuffers(1, &this->FBO);
	glGenFramebuffers(1, &this->blurFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
	this->genTexture(this->colorBuffer, GL_COLOR_ATTACHMENT0, GL_RED, GL_RED, windowWidth, windowHeight);
	
	if (!this->checkFBOStatus()) {
		std::cerr << "Error initializing SSAO FBO" << std::endl;
		exit(-1);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, this->blurFBO);
	this->genTexture(this->colorBufferBlur, GL_COLOR_ATTACHMENT0, GL_RED, GL_RED, windowWidth, windowHeight);

	if (!this->checkFBOStatus()) {
		std::cerr << "Error initializing SSAO blur FBO" << std::endl;
		exit(-1);
	}
}

void SSAO::calcSSAO(GLuint gPosition, GLuint gNormal, const glm::mat4& projection, GLuint currFramebuffer) {
	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
	glClear(GL_COLOR_BUFFER_BIT);

	this->shader.useShader();

	this->shader.setMat4("projection", projection);
	this->shader.setInt("gPosition", 0);
	this->shader.setInt("gNormal", 1);
	this->shader.setInt("noise", 2);

	this->shader.setVec2("screenRes", this->scrResolution);
	this->shader.setInt("kernelSize", 64);
	this->shader.setFloat("radius", 0.5f);
	this->shader.setFloat("bias", 0.035f);
	this->shader.setFloat("occlusionPower", 100.f);

	std::string buffer{};
	for (uint i = 0; i < 64; i++) {
		buffer = "samples[" + std::to_string(i) + "]";
		this->shader.setVec3(buffer.data(), this->kernel[i]);
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, this->noiseTexture);

	this->quad.renderQuad();

	this->shader.endShader();

	this->ssaoBlur();

	glBindFramebuffer(GL_FRAMEBUFFER, currFramebuffer);
}

void SSAO::ssaoBlur() {
	glBindFramebuffer(GL_FRAMEBUFFER, this->blurFBO);
	glClear(GL_COLOR_BUFFER_BIT);

	this->blurShader.useShader();
	this->blurShader.setInt("ssao", 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->colorBuffer);

	this->quad.renderQuad();

	this->blurShader.endShader();
}