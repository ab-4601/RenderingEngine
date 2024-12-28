#include "SSAO.h"

SSAO::SSAO(int windowWidth, int windowHeight) {
	kernel.clear();
	noise.clear();

	quad.createQuad();
	scrResolution = glm::vec2(windowWidth, windowHeight);

	// sample kernel generation
	for (uint i = 0; i < 64; i++) {
		glm::vec3 sample(
			jitter(0.f, 1.f) * 2.f - 1.f,
			jitter(0.f, 1.f) * 2.f - 1.f,
			jitter(0.f, 1.f)
		);

		if (i < 16) {
			glm::vec3 noise{
				jitter(0.f, 1.f) * 2.f - 1.f,
				jitter(0.f, 1.f) * 2.f - 1.f,
				0.f
			};

			noise = glm::normalize(noise);
			this->noise.push_back(noise);
		}

		sample = glm::normalize(sample);
		sample *= jitter(0.f, 1.f);
		float scale = (float)i / 64.f;

		scale = lerp(0.1f, 1.f, scale * scale);
		sample *= scale;

		kernel.push_back(sample);
	}

	_init(windowWidth, windowHeight);
	genNoiseTexture(windowWidth, windowHeight);
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
	glGenTextures(1, &noiseTexture);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGBA, GL_FLOAT, noise.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void SSAO::_init(int windowWidth, int windowHeight) {
	glGenFramebuffers(1, &FBO);
	glGenFramebuffers(1, &blurFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	genTexture(colorBuffer, GL_COLOR_ATTACHMENT0, GL_RED, GL_RED, windowWidth, windowHeight);
	
	if (!checkFBOStatus()) {
		std::cerr << "Error initializing SSAO FBO" << std::endl;
		exit(-1);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, blurFBO);
	genTexture(colorBufferBlur, GL_COLOR_ATTACHMENT0, GL_RED, GL_RED, windowWidth, windowHeight);

	if (!checkFBOStatus()) {
		std::cerr << "Error initializing SSAO blur FBO" << std::endl;
		exit(-1);
	}
}

void SSAO::calcSSAO(GLuint gPosition, GLuint gNormal, GLuint currFramebuffer) {
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glClear(GL_COLOR_BUFFER_BIT);

	shader.useShader();

	shader.setVec2("screenRes", scrResolution);
	shader.setInt("kernelSize", 64);
	shader.setFloat("radius", radius);
	shader.setFloat("bias", bias);
	shader.setFloat("occlusionPower", occlusionPower);

	char buffer[30]{ '\0' };
	for (uint i = 0; i < 64; i++) {
		snprintf(buffer, sizeof(buffer), "samples[%i]", i);
		shader.setVec3(buffer, kernel[i]);
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);

	quad.renderQuad();

	shader.endShader();

	glActiveTexture(GL_TEXTURE0);

	ssaoBlur();

	glBindFramebuffer(GL_FRAMEBUFFER, currFramebuffer);
}

void SSAO::ssaoBlur() {
	glBindFramebuffer(GL_FRAMEBUFFER, blurFBO);
	glClear(GL_COLOR_BUFFER_BIT);

	blurShader.useShader();
	blurShader.setInt("ssao", 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);

	quad.renderQuad();

	blurShader.endShader();
}