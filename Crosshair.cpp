#include "Crosshair.h"

Crosshair::Crosshair()
	: VAO{ 0 }, VBO{ 0 } {

	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->vertices), this->vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	this->crosshairTexture.loadTexture();
}

void Crosshair::drawCrosshair() {
	glClear(GL_DEPTH_BUFFER_BIT);
	this->shader.useShader();

	glm::mat4 transformation(1.f);
	transformation = glm::scale(transformation, glm::vec3(0.009f, 0.015f, 0.009f));

	this->shader.setMat4("transformation", transformation);
	this->shader.setInt("textureSampler", 0);

	this->crosshairTexture.useTexture();

	glBindVertexArray(this->VAO);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, sizeof(this->vertices) / sizeof(float));

	glBindVertexArray(0);

	this->shader.endShader();
}

void Crosshair::clearMesh() {
	if (this->VBO != 0) {
		glDeleteBuffers(1, &this->VBO);
		this->VBO = 0;
	}

	if (this->VAO != 0) {
		glDeleteVertexArrays(1, &this->VAO);
		this->VAO = 0;
	}
}