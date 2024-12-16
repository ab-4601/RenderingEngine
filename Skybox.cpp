#include "Skybox.h"

Skybox::Skybox(int windowWidth, int windowHeight, const char* fileName)
	: Texture() {
	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->vertices), this->vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	this->quad.createQuad();

	this->_initFBO();
	this->loadEquirectangularMap(fileName);
	this->_generateCubemap();
	this->_generateIrradianceMap();
	this->_generatePrefilterMipmap();
	this->_generateBRDFMap();

	glViewport(0, 0, windowWidth, windowHeight);
}

void Skybox::_initFBO() {
	glGenFramebuffers(1, &this->FBO);
	glGenRenderbuffers(1, &this->RBO);

	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
	glBindRenderbuffer(GL_RENDERBUFFER, this->RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, this->CUBEMAP_RESOLUTION, this->CUBEMAP_RESOLUTION);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->RBO);
}

void Skybox::loadEquirectangularMap(const char* fileName) {
	stbi_set_flip_vertically_on_load(true);

	float* data = stbi_loadf(fileName, &this->width, &this->height, &this->bitDepth, 0);

	if (data) {
		glGenTextures(1, &this->textureID);
		glBindTexture(GL_TEXTURE_2D, this->textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, this->width, this->height, 0, GL_RGB, GL_FLOAT, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else {
		std::cerr << "Unable to load HDR image" << std::endl;
	}

	stbi_set_flip_vertically_on_load(false);
}

void Skybox::_generateCubemap() {
	glGenTextures(1, &this->environmentMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->environmentMap);

	for (int i = 0; i < 6; i++) {
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F,
			this->CUBEMAP_RESOLUTION, this->CUBEMAP_RESOLUTION, 0, GL_RGB, GL_FLOAT, NULL
		);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	this->hdrToCubeShader.useShader();

	this->hdrToCubeShader.setMat4("projection", this->projection);
	this->hdrToCubeShader.setInt("equirectangularMap", 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->textureID);

	glViewport(0, 0, this->CUBEMAP_RESOLUTION, this->CUBEMAP_RESOLUTION);
	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);

	for (int i = 0; i < 6; i++) {
		this->hdrToCubeShader.setMat4("view", this->viewMatrices[i]);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, this->environmentMap, 0
		);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		this->renderCube();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindTexture(GL_TEXTURE_CUBE_MAP, this->environmentMap);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	this->hdrToCubeShader.endShader();
}

void Skybox::_generateIrradianceMap() {
	glGenTextures(1, &this->irradianceMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->irradianceMap);

	for (int i = 0; i < 6; i++)
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F,
			this->CONVOLUTION_RESOLUTION, this->CONVOLUTION_RESOLUTION, 0, GL_RGB, GL_FLOAT, NULL
		);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
	glBindRenderbuffer(GL_RENDERBUFFER, this->RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, this->CONVOLUTION_RESOLUTION, this->CONVOLUTION_RESOLUTION);

	this->irradianceShader.useShader();
	this->irradianceShader.setInt("environmentMap", 0);
	this->irradianceShader.setMat4("projection", this->projection);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->environmentMap);

	glViewport(0, 0, this->CONVOLUTION_RESOLUTION, this->CONVOLUTION_RESOLUTION);
	for (int i = 0; i < 6; i++) {
		this->irradianceShader.setMat4("view", this->viewMatrices[i]);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, this->irradianceMap, 0
		);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		this->renderCube();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	this->irradianceShader.endShader();
}

void Skybox::_generatePrefilterMipmap() {
	glGenTextures(1, &this->prefilterMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->prefilterMap);

	for (int i = 0; i < 6; i++)
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F,
			this->PREFILTER_RESOLUTION, this->PREFILTER_RESOLUTION, 0, GL_RGB, GL_FLOAT, NULL
		);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	uint maxMipLevels = 10;

	this->prefilterShader.useShader();
	this->prefilterShader.setInt("environmentMap", 0);
	this->prefilterShader.setMat4("projection", this->projection);
	this->prefilterShader.setFloat("cubemapResolution", this->CUBEMAP_RESOLUTION);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->environmentMap);

	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);

	for (uint mipLevel = 0; mipLevel < maxMipLevels; mipLevel++) {
		uint mipWidth = static_cast<uint>(this->PREFILTER_RESOLUTION * pow(0.5f, mipLevel));
		uint mipHeight = static_cast<uint>(this->PREFILTER_RESOLUTION * pow(0.5f, mipLevel));

		glBindRenderbuffer(GL_RENDERBUFFER, this->RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);

		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mipLevel / (float)(maxMipLevels - 1);
		this->prefilterShader.setFloat("roughness", roughness);

		for (uint j = 0; j < 6; j++) {
			this->prefilterShader.setMat4("view", this->viewMatrices[j]);
			glFramebufferTexture2D(
				GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, this->prefilterMap, mipLevel
			);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			this->renderCube();
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	this->prefilterShader.endShader();
}

void Skybox::_generateBRDFMap() {
	glGenTextures(1, &this->brdfTexture);
	glBindTexture(GL_TEXTURE_2D, this->brdfTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, this->CUBEMAP_RESOLUTION, this->CUBEMAP_RESOLUTION, 0, GL_RG, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
	glBindRenderbuffer(GL_RENDERBUFFER, this->RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, this->CUBEMAP_RESOLUTION, this->CUBEMAP_RESOLUTION);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->brdfTexture, 0);

	glViewport(0, 0, this->CUBEMAP_RESOLUTION, this->CUBEMAP_RESOLUTION);

	this->brdfShader.useShader();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	this->quad.renderQuad();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Skybox::renderSkybox() {
	glDepthFunc(GL_LEQUAL);

	this->skyboxShader.useShader();

	this->skyboxShader.setInt("environmentMap", 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->environmentMap);

	this->renderCube();

	glDepthFunc(GL_LESS);

	this->skyboxShader.endShader();
}