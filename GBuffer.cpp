#include "GBuffer.h"

GBuffer::GBuffer(int windowWidth, int windowHeight) {
	viewportMatrix = {
		glm::vec4((float)windowWidth / 2.f, 0.f, 0.f, (float)windowWidth / 2.f),
		glm::vec4(0.f, (float)windowHeight / 2.f, 0.f, (float)windowHeight / 2.f),
		glm::vec4(0.f, 0.f, 1.f, 0.f),
		glm::vec4(0.f, 0.f, 0.f, 1.f)
	};

	_init(windowWidth, windowHeight);

	shader.useShader();
		shader.setMat4("viewportMatrix", viewportMatrix);
	shader.endShader();
}

void GBuffer::genTexture(GLuint& texID, GLenum colorAttachment, int windowWidth, int windowHeight) {
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, colorAttachment, GL_TEXTURE_2D, texID, 0);
}

void GBuffer::_init(int windowWidth, int windowHeight) {
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	genTexture(gPosition, GL_COLOR_ATTACHMENT0, windowWidth, windowHeight);
	genTexture(gNormal, GL_COLOR_ATTACHMENT1, windowWidth, windowHeight);
	genTexture(gAlbedo, GL_COLOR_ATTACHMENT2, windowWidth, windowHeight);
	genTexture(gMetallic, GL_COLOR_ATTACHMENT3, windowWidth, windowHeight);

	glDrawBuffers(4, colorAttachments);

	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, windowWidth, windowHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

	GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Error initializing G-Buffer Framebuffer" << std::endl;
		exit(-1);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
}

void GBuffer::updateBuffer(Shader& outlineShader, int meshId, const std::vector<Mesh*>& meshes,
	const std::vector<Model*>& models, GLuint currFramebuffer)
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glDrawBuffers(4, colorAttachments);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	shader.useShader();
	shader.setUint("drawWireframe", drawWireframe);

	if (meshId < meshes.size() && meshId != -1)
		meshes[meshId]->renderMeshWithOutline(shader, outlineShader, GL_TRIANGLES);
	
	for (size_t i = 0; i < meshes.size(); i++) {
		if (i != meshId)
			meshes[i]->renderMesh(shader, GL_TRIANGLES);
	}

	for (size_t i = 0; i < models.size(); i++)
		models[i]->renderModel(shader, GL_TRIANGLES);

	shader.endShader();

	glBindFramebuffer(GL_FRAMEBUFFER, currFramebuffer);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
}