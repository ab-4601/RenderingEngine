#pragma once

#include "Core.h"
#include "Camera.h"
#include "Texture.h"
#include "Shader.h"

class Crosshair {
private:
	GLuint VAO, VBO;

	GLfloat vertices[12] {
		-1.f,  1.f,  0.f,
		-1.f, -1.f,  0.f,
		 1.f,  1.f,  0.f,
		 1.f, -1.f,  0.f
	};

	Shader shader{ "crosshair.vert", "crosshair.frag" };
	Texture crosshairTexture{ "Textures/crosshair.png" };

public:
	Crosshair();

	void drawCrosshair();

	void clearMesh();

	~Crosshair() {
		clearMesh();
	}
};