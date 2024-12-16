#include "BloomFBO.h"

bool BloomFBO::_init(int windowWidth, int windowHeight, uint iterations) {
	if (this->isInit)
		return true;

	this->mMipChain.clear();

	glGenFramebuffers(1, &this->FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);

	glm::vec2 mipSize{ (float)windowWidth, (float)windowHeight };
	glm::ivec2 mipIntSize{ windowWidth, windowHeight };

	for (uint i = 0; i < iterations; i++) {
		BloomMip mip;

		mipSize *= 0.5f;
		mipIntSize /= 2;
		mip.size = mipSize;
		mip.intSize = mipIntSize;

		glGenTextures(1, &mip.texture);
		glBindTexture(GL_TEXTURE_2D, mip.texture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F, (int)mipSize.x, (int)mipSize.y, 0, GL_RGB, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		this->mMipChain.emplace_back(mip);
	}
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->mMipChain[0].texture, 0);

	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Error initializing bloom mip framebuffer" << std::endl;
		return false;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	this->isInit = true;
	return true;
}

BloomFBO::~BloomFBO() {
	if (this->FBO != 0)
		glDeleteFramebuffers(1, &this->FBO);

	if (this->mMipChain.size() != 0) {
		for (size_t i = 0; i < this->mMipChain.size(); i++)
			glDeleteTextures(1, &this->mMipChain.at(i).texture);
	}
}