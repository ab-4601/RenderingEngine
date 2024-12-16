#pragma once

#include "Core.h"

struct BloomMip {
	glm::vec2 size{ 0 };
	glm::ivec2 intSize{ 0 };
	GLuint texture{ 0 };
};

class BloomFBO {
private:
	GLuint FBO{ 0 };
	bool isInit = false;
	std::vector<BloomMip> mMipChain{};

public:
	BloomFBO() = default;

	bool _init(int windowWidth, int windowHeight, uint iterations = 6);

	const std::vector<BloomMip>& mipChain() const {
		return this->mMipChain;
	}

	void enableWriting() const {
		glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
	}

	~BloomFBO();
};

