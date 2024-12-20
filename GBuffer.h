#pragma once

#include "Model.h"

class GBuffer {
private:
	GLuint FBO{ 0 }, RBO{ 0 }, gPosition{ 0 }, gNormal{ 0 }, gAlbedo{ 0 }, gMetallic{ 0 };

	Shader shader{ "gbuffer.vert", "gbuffer.frag" };

	GLenum colorAttachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };

	void genTexture(GLuint& texID, GLenum colorAttachment, int windowWidth, int windowHeight);
	void _init(int windowWidth, int windowHeight);

public:
	GBuffer(int windowWidth, int windowHeight);

	inline GLuint getFramebufferID() const { return this->FBO; }
	inline GLuint getPositionBuffer() const { return this->gPosition; }
	inline GLuint getNormalBuffer() const { return this->gNormal; }
	inline GLuint getAlbedoBuffer() const { return this->gAlbedo; }
	inline GLuint getMetallicBuffer() const { return this->gMetallic; }

	void updateGbuffer(PBRShader& shader, const std::vector<Mesh*>& meshes, const std::vector<Model*>& models, 
		glm::vec3 cameraPosition, GLuint currFramebuffer = 0);

	~GBuffer() {
		if (this->FBO != 0)
			glDeleteFramebuffers(1, &this->FBO);

		if (this->gPosition != 0) {
			GLuint textures[] = { gPosition, gNormal, gAlbedo, gMetallic };
			glDeleteTextures(4, textures);
		}
	}
};

