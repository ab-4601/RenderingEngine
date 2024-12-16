#include "GBuffer.h"

GBuffer::GBuffer(int windowWidth, int windowHeight) {
	this->_init(windowWidth, windowHeight);
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
	glGenFramebuffers(1, &this->FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);

	this->genTexture(this->gPosition, GL_COLOR_ATTACHMENT0, windowWidth, windowHeight);
	this->genTexture(this->gNormal, GL_COLOR_ATTACHMENT1, windowWidth, windowHeight);
	this->genTexture(this->gAlbedo, GL_COLOR_ATTACHMENT2, windowWidth, windowHeight);
	this->genTexture(this->gMetallic, GL_COLOR_ATTACHMENT3, windowWidth, windowHeight);

	glDrawBuffers(4, this->colorAttachments);

	glGenRenderbuffers(1, &this->RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, this->RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowWidth, windowHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->RBO);

	GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Error initializing G-Buffer Framebuffer" << std::endl;
		exit(-1);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
}

void GBuffer::updateGbuffer(PBRShader& shader, const std::vector<Mesh*>& meshes, const std::vector<Model*>& models,
	glm::vec3 cameraPosition, GLuint currFramebuffer)
{
	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
	glDrawBuffers(4, this->colorAttachments);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	this->shader.useShader();

	this->shader.setInt("diffuseMap", 0);
	this->shader.setInt("normalMap", 1);
	this->shader.setInt("metalnessMap", 3);
	
	for (size_t i = 0; i < meshes.size(); i++) {
		this->shader.setVec3("vColor", meshes[i]->getColor());
		this->shader.setMat4("model", meshes[i]->getModelMatrix());

		if (meshes[i]->getDiffuseMapBool()) {
			this->shader.setUint("useDiffuseMap", true);
			meshes[i]->bindDiffuseMap();
		}
		else
			this->shader.setUint("useDiffuseMap", false);

		if (meshes[i]->getNormalMapBool()) {
			this->shader.setUint("useNormalMap", true);
			meshes[i]->bindNormalMap();
		}
		else
			this->shader.setUint("useNormalMap", false);

		if (meshes[i]->getMaterialMapBool()) {
			this->shader.setUint("useMetalnessMap", true);
			meshes[i]->bindMetallicMap();
		}
		else
			this->shader.setUint("useMetalnessMap", false);

		meshes[i]->drawMesh(GL_TRIANGLES);
	}

	for (size_t i = 0; i < models.size(); i++) {
		this->shader.setUint("useDiffuseMap", true);
		this->shader.setUint("useNormalMap", true);
		this->shader.setUint("useMetalnessMap", true);
		this->shader.setMat4("model", glm::mat4(1.f));
		models[i]->renderModel(shader, cameraPosition);
	}

	this->shader.endShader();

	glBindFramebuffer(GL_FRAMEBUFFER, currFramebuffer);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
}