#pragma once

#include "Shader.h"
#include "Texture.h"
#include "Quad.h"

class HDR {
private:
	GLuint FBO = 0;
	GLuint colorBuffer = 0;
	GLuint depthBuffer = 0;

	GLuint intermediateFBO = 0;
	GLuint screenBuffer = 0;

	GLuint edgeFBO = 0, blendFBO = 0;
	GLuint edgeColorBuffer = 0, blendColorBuffer = 0;

	Texture areaTexture{ "Textures/SMAA/AreaTexDX10.dds" }, searchTexture{ "Textures/SMAA/SearchTex.dds" };

	glm::ivec2 screenResolution;

	Shader shader{ "hdr.vert", "hdr.frag" };
	Shader edgeShader{ "smaa.vert", "smaa_edge.frag" };
	Shader blendShader{ "smaa.vert", "smaa_blend.frag" };

	Quad quad;

	bool checkFramebufferStatus();

	void _initIntermediateFBO();
	void _initSMAAbuffers();

	void calcSMAAbuffers(GLuint screenTexture);

public:
	HDR(int windowWidth = 800, int windowHeight = 800);

	void _init();
	void _initMSAA();

	GLuint getFramebufferID() const { return FBO; }
	GLuint getColorbufferID() const { return colorBuffer; }

	void enableHDRWriting() const { glBindFramebuffer(GL_FRAMEBUFFER, FBO); }

	void renderToDefaultBuffer(float exposure = 1.f, GLuint bloomBuffer = 0, bool enableBloom = false);
	void renderToDefaultBufferMSAA(
		float exposure = 1.f, GLuint bloomBuffer = 0, bool enableBloom = false
	);

	~HDR();
};