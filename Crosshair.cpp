#include "Crosshair.h"

Crosshair::Crosshair()
	: VAO{ 0 }, VBO{ 0 } {

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	crosshairTexture.loadTexture();
}

void Crosshair::drawCrosshair() {
	glClear(GL_DEPTH_BUFFER_BIT);
	shader.useShader();

	glm::mat4 transformation(1.f);
	transformation = glm::scale(transformation, glm::vec3(0.009f, 0.015f, 0.009f));

	shader.setMat4("transformation", transformation);
	shader.setInt("textureSampler", 0);

	crosshairTexture.useTexture();

	glBindVertexArray(VAO);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, sizeof(vertices) / sizeof(float));

	glBindVertexArray(0);

	shader.endShader();
}

void Crosshair::clearMesh() {
	if (VBO != 0) {
		glDeleteBuffers(1, &VBO);
		VBO = 0;
	}

	if (VAO != 0) {
		glDeleteVertexArrays(1, &VAO);
		VAO = 0;
	}
}