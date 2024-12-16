#include "HDR.h"

HDR::HDR(int windowWidth, int windowHeight) {
	this->screenResolution = glm::ivec2(windowWidth, windowHeight);
	this->quad.createQuad();
}

bool HDR::checkFramebufferStatus() {
	GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE)
		return false;

	return true;
}

void HDR::_init() {
	glGenFramebuffers(1, &this->FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);

	glGenTextures(1, &this->colorBuffer);

	glBindTexture(GL_TEXTURE_2D, this->colorBuffer);
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGBA16F, this->screenResolution.x, this->screenResolution.y, 0, GL_RGBA, GL_FLOAT, NULL
	);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->colorBuffer, 0);

	glGenRenderbuffers(1, &this->depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, this->depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, this->screenResolution.x, this->screenResolution.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->depthBuffer);

	if (!this->checkFramebufferStatus()) {
		std::cerr << "HDR framebuffer initialization error" << std::endl;
		exit(-1);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void HDR::_initMSAA() {
	glGenFramebuffers(1, &this->FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);

	glGenTextures(1, &this->colorBuffer);

	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, this->colorBuffer);

	glTexImage2DMultisample(
		GL_TEXTURE_2D_MULTISAMPLE, ::samples, GL_RGBA16F, this->screenResolution.x, this->screenResolution.y, GL_TRUE
	);

	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, this->colorBuffer, 0
	);

	glGenRenderbuffers(1, &this->depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, this->depthBuffer);
	glRenderbufferStorageMultisample(
		GL_RENDERBUFFER, ::samples, GL_DEPTH24_STENCIL8, this->screenResolution.x, this->screenResolution.y
	);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->depthBuffer);

	if (!this->checkFramebufferStatus()) {
		std::cerr << "HDR MSAA Framebuffer initialization error" << std::endl;
		exit(-1);
	}

	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	this->_initIntermediateFBO();
}

void HDR::_initSMAAbuffers() {
	glGenFramebuffers(1, &this->edgeFBO);
	glGenFramebuffers(1, &this->blendFBO);

	glGenTextures(1, &this->edgeColorBuffer);
	glBindTexture(GL_TEXTURE_2D, this->edgeColorBuffer);
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGBA16F, this->screenResolution.x, this->screenResolution.y, 0, GL_RGBA, GL_FLOAT, NULL
	);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, this->edgeFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->edgeColorBuffer, 0);

	if (!this->checkFramebufferStatus()) {
		std::cerr << "Error initializing SMAA edge detection FBO" << std::endl;
		exit(-1);
	}

	glGenTextures(1, &this->blendColorBuffer);
	glBindTexture(GL_TEXTURE_2D, this->blendColorBuffer);
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGBA16F, this->screenResolution.x, this->screenResolution.y, 0, GL_RGBA, GL_FLOAT, NULL
	);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, this->blendFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->blendColorBuffer, 0);

	if (!this->checkFramebufferStatus()) {
		std::cerr << "Error initializing SMAA blend FBO" << std::endl;
		exit(-1);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	this->_initSMAAbuffers();
}

void HDR::_initIntermediateFBO() {
	glGenFramebuffers(1, &this->intermediateFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, this->intermediateFBO);

	glGenTextures(1, &this->screenBuffer);

	glBindTexture(GL_TEXTURE_2D, this->screenBuffer);
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGBA16F, this->screenResolution.x, this->screenResolution.y, 0, GL_RGBA, GL_FLOAT, NULL
	);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->screenBuffer, 0);

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
	glBindFramebuffer(GL_FRAMEBUFFER, this->edgeFBO);
	glClear(GL_COLOR_BUFFER_BIT);

	this->edgeShader.useShader();

	this->edgeShader.setInt("screenTexture", 0);
	this->edgeShader.setVec2("screenResolution", this->screenResolution);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, screenTexture);

	this->quad.renderQuad();

	this->edgeShader.endShader();

	// blend weight calculation pass
	glBindFramebuffer(GL_FRAMEBUFFER, this->blendFBO);
	glClear(GL_COLOR_BUFFER_BIT);

	this->blendShader.useShader();

	this->blendShader.setInt("edgeTexture", 0);
	this->blendShader.setVec2("screenResolution", this->screenResolution);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->edgeColorBuffer);

	this->quad.renderQuad();

	this->blendShader.endShader();
}

void HDR::renderToDefaultBuffer(float exposure, GLuint bloomBuffer, bool enableBloom) {
	//this->calcSMAAbuffers(this->colorBuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	this->shader.useShader();

	this->shader.setFloat("exposure", exposure);
	this->shader.setInt("enableBloom", enableBloom);
	this->shader.setVec2("screenResolution", this->screenResolution);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->colorBuffer);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bloomBuffer);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, this->blendColorBuffer);

	this->quad.renderQuad();

	glActiveTexture(GL_TEXTURE0);

	this->shader.endShader();
}

void HDR::renderToDefaultBufferMSAA(float exposure, GLuint bloomBuffer, bool enableBloom) 
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, this->FBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->intermediateFBO);
	glBlitFramebuffer(
		0, 0, this->screenResolution.x, this->screenResolution.y,
		0, 0, this->screenResolution.x, this->screenResolution.y,
		GL_COLOR_BUFFER_BIT, GL_NEAREST
	);

	//this->calcSMAAbuffers(this->screenBuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	this->shader.useShader();

	this->shader.setFloat("exposure", exposure);
	this->shader.setInt("enableBloom", enableBloom);
	this->shader.setVec2("screenResolution", this->screenResolution);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->screenBuffer);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bloomBuffer);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, this->blendColorBuffer);

	this->quad.renderQuad();

	glActiveTexture(GL_TEXTURE0);

	this->shader.endShader();
}

HDR::~HDR() {
	if (this->FBO != 0)
		glDeleteFramebuffers(1, &this->FBO);

	if (this->intermediateFBO != 0)
		glDeleteFramebuffers(1, &this->intermediateFBO);

	if (this->screenBuffer != 0)
		glDeleteTextures(1, &this->screenBuffer);

	if (this->colorBuffer != 0)
		glDeleteTextures(1, &this->colorBuffer);

	if (this->depthBuffer != 0)
		glDeleteRenderbuffers(1, &this->depthBuffer);
}