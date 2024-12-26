#pragma once

#include "Core.h"

class SelectionTexture {
private:
	GLuint FBO = 0;
	GLuint selectionTexture = 0;
	GLuint depthTexture = 0;
	GLuint depthBuffer = 0;

public:
	SelectionTexture() = default;

	void init(unsigned int windowWidth, unsigned int windowHeight);

	void enableWriting() const { glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->FBO); }
	void disableWriting(GLuint framebuffer = 0) const { glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer); }

	struct PixelInfo {
		GLuint objectID = 0;
		GLuint drawID = 0;
		GLuint primitiveID = 0;
	};

	PixelInfo readPixel(int x, int y);

	~SelectionTexture();
};
