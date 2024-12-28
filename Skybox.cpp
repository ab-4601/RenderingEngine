#include "Skybox.h"

Skybox::Skybox(int windowWidth, int windowHeight, const char* fileName)
	: Texture() {
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	quad.createQuad();

	_initFBO();
	loadEquirectangularMap(fileName);
	_generateCubemap();
	_generateIrradianceMap();
	_generatePrefilterMipmap();
	_generateBRDFMap();

	glViewport(0, 0, windowWidth, windowHeight);
}

void Skybox::_initFBO() {
	glGenFramebuffers(1, &FBO);
	glGenRenderbuffers(1, &RBO);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, CUBEMAP_RESOLUTION, CUBEMAP_RESOLUTION);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);
}

void Skybox::loadEquirectangularMap(const char* fileName) {
	stbi_set_flip_vertically_on_load(true);

	float* data = stbi_loadf(fileName, &width, &height, &bitDepth, 0);

	if (data) {
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, data);

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
	glGenTextures(1, &environmentMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, environmentMap);

	for (int i = 0; i < 6; i++) {
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F,
			CUBEMAP_RESOLUTION, CUBEMAP_RESOLUTION, 0, GL_RGB, GL_FLOAT, NULL
		);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	hdrToCubeShader.useShader();

	hdrToCubeShader.setMat4("projection", projection);
	hdrToCubeShader.setInt("equirectangularMap", 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glViewport(0, 0, CUBEMAP_RESOLUTION, CUBEMAP_RESOLUTION);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	for (int i = 0; i < 6; i++) {
		hdrToCubeShader.setMat4("view", viewMatrices[i]);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, environmentMap, 0
		);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderCube();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindTexture(GL_TEXTURE_CUBE_MAP, environmentMap);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	hdrToCubeShader.endShader();
}

void Skybox::_generateIrradianceMap() {
	glGenTextures(1, &irradianceMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);

	for (int i = 0; i < 6; i++)
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F,
			CONVOLUTION_RESOLUTION, CONVOLUTION_RESOLUTION, 0, GL_RGB, GL_FLOAT, NULL
		);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, CONVOLUTION_RESOLUTION, CONVOLUTION_RESOLUTION);

	irradianceShader.useShader();
	irradianceShader.setInt("environmentMap", 0);
	irradianceShader.setMat4("projection", projection);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, environmentMap);

	glViewport(0, 0, CONVOLUTION_RESOLUTION, CONVOLUTION_RESOLUTION);
	for (int i = 0; i < 6; i++) {
		irradianceShader.setMat4("view", viewMatrices[i]);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0
		);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderCube();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	irradianceShader.endShader();
}

void Skybox::_generatePrefilterMipmap() {
	glGenTextures(1, &prefilterMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);

	for (int i = 0; i < 6; i++)
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F,
			PREFILTER_RESOLUTION, PREFILTER_RESOLUTION, 0, GL_RGB, GL_FLOAT, NULL
		);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	uint maxMipLevels = 10;

	prefilterShader.useShader();
	prefilterShader.setInt("environmentMap", 0);
	prefilterShader.setMat4("projection", projection);
	prefilterShader.setFloat("cubemapResolution", CUBEMAP_RESOLUTION);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, environmentMap);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	for (uint mipLevel = 0; mipLevel < maxMipLevels; mipLevel++) {
		uint mipWidth = static_cast<uint>(PREFILTER_RESOLUTION * pow(0.5f, mipLevel));
		uint mipHeight = static_cast<uint>(PREFILTER_RESOLUTION * pow(0.5f, mipLevel));

		glBindRenderbuffer(GL_RENDERBUFFER, RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);

		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mipLevel / (float)(maxMipLevels - 1);
		prefilterShader.setFloat("roughness", roughness);

		for (uint j = 0; j < 6; j++) {
			prefilterShader.setMat4("view", viewMatrices[j]);
			glFramebufferTexture2D(
				GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, prefilterMap, mipLevel
			);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			renderCube();
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	prefilterShader.endShader();
}

void Skybox::_generateBRDFMap() {
	glGenTextures(1, &brdfTexture);
	glBindTexture(GL_TEXTURE_2D, brdfTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, CUBEMAP_RESOLUTION, CUBEMAP_RESOLUTION, 0, GL_RG, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, CUBEMAP_RESOLUTION, CUBEMAP_RESOLUTION);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfTexture, 0);

	glViewport(0, 0, CUBEMAP_RESOLUTION, CUBEMAP_RESOLUTION);

	brdfShader.useShader();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	quad.renderQuad();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Skybox::renderSkybox() {
	glDepthFunc(GL_LEQUAL);

	skyboxShader.useShader();

	skyboxShader.setInt("environmentMap", 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, environmentMap);

	renderCube();

	glDepthFunc(GL_LESS);

	skyboxShader.endShader();
}