#include "SelectionTexture.h"

void SelectionTexture::init(unsigned int windowWidth, unsigned int windowHeight) {
	glGenFramebuffers(1, &this->FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);

	glGenTextures(1, &this->selectionTexture);
	glBindTexture(GL_TEXTURE_2D, this->selectionTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32UI, windowWidth, windowHeight, 0, GL_RGB_INTEGER, GL_UNSIGNED_INT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->selectionTexture, 0);

	glGenTextures(1, &this->depthTexture);
	glBindTexture(GL_TEXTURE_2D, this->depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, windowWidth, windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->depthTexture, 0);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Framebuffer error. Exiting program" << std::endl;
		exit(1);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

SelectionTexture::PixelInfo SelectionTexture::readPixel(int x, int y) {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, this->FBO);
	glReadBuffer(GL_COLOR_ATTACHMENT0);

	PixelInfo Pixel;
	glReadPixels(x, y, 1, 1, GL_RGB_INTEGER, GL_UNSIGNED_INT, &Pixel);

	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	return Pixel;
}

SelectionTexture::~SelectionTexture() {
	if (this->FBO != 0)
		glDeleteFramebuffers(1, &this->FBO);

	if (this->selectionTexture != 0)
		glDeleteTextures(1, &this->selectionTexture);

	if (this->depthTexture != 0)
		glDeleteTextures(1, &this->depthTexture);

	if (this->depthBuffer != 0)
		glDeleteRenderbuffers(1, &this->depthBuffer);
}