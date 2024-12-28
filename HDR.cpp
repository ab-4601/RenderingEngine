#include "HDR.h"

HDR::HDR(int windowWidth, int windowHeight) {
	screenResolution = glm::ivec2(windowWidth, windowHeight);
	quad.createQuad();
}

bool HDR::checkFramebufferStatus() {
	GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE)
		return false;

	return true;
}

void HDR::_init() {
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &colorBuffer);

	glBindTexture(GL_TEXTURE_2D, colorBuffer);
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGBA16F, screenResolution.x, screenResolution.y, 0, GL_RGBA, GL_FLOAT, NULL
	);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);

	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenResolution.x, screenResolution.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

	if (!checkFramebufferStatus()) {
		std::cerr << "HDR framebuffer initialization error" << std::endl;
		exit(-1);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	_initSMAAbuffers();
}

void HDR::_initMSAA() {
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &colorBuffer);

	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, colorBuffer);

	glTexImage2DMultisample(
		GL_TEXTURE_2D_MULTISAMPLE, ::samples, GL_RGBA16F, screenResolution.x, screenResolution.y, GL_TRUE
	);

	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, colorBuffer, 0
	);

	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorageMultisample(
		GL_RENDERBUFFER, ::samples, GL_DEPTH24_STENCIL8, screenResolution.x, screenResolution.y
	);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

	if (!checkFramebufferStatus()) {
		std::cerr << "HDR MSAA Framebuffer initialization error" << std::endl;
		exit(-1);
	}

	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	_initIntermediateFBO();
	_initSMAAbuffers();
}

void HDR::_initSMAAbuffers() {
	glGenFramebuffers(1, &edgeFBO);
	glGenFramebuffers(1, &blendFBO);

	glGenTextures(1, &edgeColorBuffer);
	glBindTexture(GL_TEXTURE_2D, edgeColorBuffer);
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RG8, screenResolution.x, screenResolution.y, 0, GL_RG, GL_UNSIGNED_BYTE, NULL
	);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, edgeFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, edgeColorBuffer, 0);

	if (!checkFramebufferStatus()) {
		std::cerr << "Error initializing SMAA edge detection FBO" << std::endl;
		exit(-1);
	}

	glGenTextures(1, &blendColorBuffer);
	glBindTexture(GL_TEXTURE_2D, blendColorBuffer);
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGBA16F, screenResolution.x, screenResolution.y, 0, GL_RGBA, GL_FLOAT, NULL
	);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, blendFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blendColorBuffer, 0);

	if (!checkFramebufferStatus()) {
		std::cerr << "Error initializing SMAA blend FBO" << std::endl;
		exit(-1);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void HDR::_initIntermediateFBO() {
	glGenFramebuffers(1, &intermediateFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);

	glGenTextures(1, &screenBuffer);

	glBindTexture(GL_TEXTURE_2D, screenBuffer);
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGBA16F, screenResolution.x, screenResolution.y, 0, GL_RGBA, GL_FLOAT, NULL
	);

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

void HDR::calcSMAAbuffers(GLuint screenTexture) {
	// edge calculation pass
	glBindFramebuffer(GL_FRAMEBUFFER, edgeFBO);
	glClear(GL_COLOR_BUFFER_BIT);

	edgeShader.useShader();

	edgeShader.setInt("screenTexture", 0);
	edgeShader.setVec2("screenResolution", screenResolution);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, screenTexture);

	quad.renderQuad();

	edgeShader.endShader();

	// blend weight calculation pass
	glBindFramebuffer(GL_FRAMEBUFFER, blendFBO);
	glClear(GL_COLOR_BUFFER_BIT);

	blendShader.useShader();

	blendShader.setInt("edgeTexture", 0);
	blendShader.setVec2("screenResolution", screenResolution);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, edgeColorBuffer);

	quad.renderQuad();

	blendShader.endShader();
}

void HDR::renderToDefaultBuffer(float exposure, GLuint bloomBuffer, bool enableBloom) {
	//this->calcSMAAbuffers(this->colorBuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	shader.useShader();

	shader.setFloat("exposure", exposure);
	shader.setInt("enableBloom", enableBloom);
	shader.setVec2("screenResolution", screenResolution);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bloomBuffer);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, blendColorBuffer);

	quad.renderQuad();

	glActiveTexture(GL_TEXTURE0);

	shader.endShader();
}

void HDR::renderToDefaultBufferMSAA(float exposure, GLuint bloomBuffer, bool enableBloom)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);
	glBlitFramebuffer(
		0, 0, screenResolution.x, screenResolution.y,
		0, 0, screenResolution.x, screenResolution.y,
		GL_COLOR_BUFFER_BIT, GL_NEAREST
	);

	//this->calcSMAAbuffers(this->screenBuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	shader.useShader();

	shader.setFloat("exposure", exposure);
	shader.setInt("enableBloom", enableBloom);
	shader.setiVec2("screenResolution", screenResolution);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, screenBuffer);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bloomBuffer);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, blendColorBuffer);

	quad.renderQuad();

	glActiveTexture(GL_TEXTURE0);

	shader.endShader();
}

HDR::~HDR() {
	if (FBO != 0)
		glDeleteFramebuffers(1, &FBO);

	if (intermediateFBO != 0)
		glDeleteFramebuffers(1, &intermediateFBO);

	if (screenBuffer != 0)
		glDeleteTextures(1, &screenBuffer);

	if (colorBuffer != 0)
		glDeleteTextures(1, &colorBuffer);

	if (depthBuffer != 0)
		glDeleteRenderbuffers(1, &depthBuffer);
}