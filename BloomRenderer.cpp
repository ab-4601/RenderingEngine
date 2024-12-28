#include "BloomRenderer.h"

BloomRenderer::BloomRenderer(int windowWidth, int windowHeight) {
	quad.createQuad();

	_init(windowWidth, windowHeight);
}

void BloomRenderer::renderDownsamples(GLuint srcTexture) {
	const std::vector<BloomMip>& mipChain = mFBO.mipChain();

	downsampleShader.useShader();

	downsampleShader.setInt("srcTexture", 0);
	downsampleShader.setVec2("srcResolution", srcViewportSizeFLT);
	downsampleShader.setInt("mipLevel", 0);
	downsampleShader.setFloat("bloomThreshold", knee);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, srcTexture);

	glDisable(GL_BLEND);
	for (size_t i = 0; i < mipChain.size(); i++) {
		const BloomMip& mip = mipChain[i];
		
		glViewport(0, 0, (GLsizei)mip.size.x, (GLsizei)mip.size.y);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mip.texture, 0
		);

		quad.renderQuad();

		downsampleShader.setVec2("srcResolution", mip.size);

		glBindTexture(GL_TEXTURE_2D, mip.texture);
	}

	downsampleShader.endShader();
}

void BloomRenderer::renderUpsamples(float filterRadius) {
	const std::vector<BloomMip>& mipChain = mFBO.mipChain();

	upsampleShader.useShader();

	upsampleShader.setInt("srcTexture", 0);
	upsampleShader.setFloat("filterRadius", filterRadius);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	for (int i = (int)mipChain.size() - 1; i > 0; i--) {
		const BloomMip& mip = mipChain[i];
		const BloomMip& nextMip = mipChain[i - 1];

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mip.texture);

		glViewport(0, 0, (GLsizei)nextMip.size.x, (GLsizei)nextMip.size.y);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, nextMip.texture, 0
		);

		quad.renderQuad();
	}

	glDisable(GL_BLEND);

	upsampleShader.endShader();
}

void BloomRenderer::_initIntermediateFBO(int width, int height) {
	glGenFramebuffers(1, &intermediateFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);

	glGenTextures(1, &screenBuffer);

	glBindTexture(GL_TEXTURE_2D, screenBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenBuffer, 0);

	GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Intermediate framebuffer initialization error" << std::endl;
		exit(-1);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool BloomRenderer::_init(int windowWidth, int windowHeight)
{
	if (isInit) return true;

	srcViewportSize = glm::ivec2(windowWidth, windowHeight);
	srcViewportSizeFLT = glm::vec2((float)windowWidth, (float)windowHeight);

	const uint iterations = 6;
	bool status = mFBO._init(windowWidth, windowHeight, iterations);

	if (!status) {
		exit(-1);
	}

	_initIntermediateFBO(windowWidth, windowHeight);

	isInit = true;
	return true;
}

void BloomRenderer::renderBloomTexture(GLuint srcTexture, float filterRadius, GLuint currFramebuffer) {
	mFBO.enableWriting();

	renderDownsamples(srcTexture);
	renderUpsamples(filterRadius);

	glBindFramebuffer(GL_FRAMEBUFFER, currFramebuffer);

	glViewport(0, 0, srcViewportSize.x, srcViewportSize.y);
}

void BloomRenderer::renderBloomTextureMSAA(float filterRadius, GLuint currFramebuffer) {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, currFramebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);
	glBlitFramebuffer(
		0, 0, srcViewportSize.x, srcViewportSize.y,
		0, 0, srcViewportSize.x, srcViewportSize.y,
		GL_COLOR_BUFFER_BIT, GL_NEAREST
	);

	mFBO.enableWriting();

	renderDownsamples(screenBuffer);
	renderUpsamples(filterRadius);

	glBindFramebuffer(GL_FRAMEBUFFER, currFramebuffer);

	glViewport(0, 0, srcViewportSize.x, srcViewportSize.y);
}