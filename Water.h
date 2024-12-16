#pragma once

#include "Core.h"
#include "WaterShader.h"
#include "Camera.h"

class Water {
private:
	GLuint VAO, VBO, IBO;

	GLfloat vertices[12] {
		0.f,  0.f,  0.f,
		10.f, 0.f,  0.f,
		0.f,  0.f,  10.f,
		10.f, 0.f,  10.f
	};

	unsigned int indices[6] {
		0, 1, 2,
		1, 2, 3
	};

	WaterShader shader;

public:
	Water();

	GLuint createFrameBuffer();
	GLuint createTextureAttachment(int width, int height);
	void render(const glm::mat4& projection, glm::mat4& model, const Camera& camera);

	~Water() {
		if (this->VAO != 0)
			glDeleteVertexArrays(1, &this->VAO);

		if (this->VBO != 0)
			glDeleteBuffers(1, &this->VBO);

		if (this->IBO != 0)
			glDeleteBuffers(1, &this->IBO);
	}
};

