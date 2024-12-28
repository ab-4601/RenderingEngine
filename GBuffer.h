#pragma once

#include "Model.h"

class GBuffer {
private:
	GLuint FBO{ 0 }, RBO{ 0 }, gPosition{ 0 }, gNormal{ 0 }, gAlbedo{ 0 }, gMetallic{ 0 };

	bool drawWireframe{ false };
	glm::mat4 viewportMatrix{ 1.f };

	Shader shader{ "gbuffer.vert", "gbuffer.frag", "gbuffer.geom" };

	GLenum colorAttachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };

	void genTexture(GLuint& texID, GLenum colorAttachment, int windowWidth, int windowHeight);
	void _init(int windowWidth, int windowHeight);

public:
	GBuffer(int windowWidth, int windowHeight);

	GLuint framebufferID() const { return FBO; }
	GLuint positionBuffer() const { return gPosition; }
	GLuint normalBuffer() const { return gNormal; }
	GLuint albedoBuffer() const { return gAlbedo; }
	GLuint metallicBuffer() const { return gMetallic; }

	void updateWireframeBool(bool drawWireframe) { this->drawWireframe = drawWireframe; }
	void updateBuffer(Shader& outlineShader, int meshID, const std::vector<Mesh*>& meshes,
		const std::vector<Model*>& models, GLuint currFramebuffer = 0);

	~GBuffer() {
		if (FBO != 0)
			glDeleteFramebuffers(1, &FBO);

		if (gPosition != 0) {
			GLuint textures[] = { gPosition, gNormal, gAlbedo, gMetallic };
			glDeleteTextures(4, textures);
		}
	}
};

