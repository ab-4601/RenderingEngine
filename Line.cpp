#include "Line.h"

Line::Line(glm::vec3 a, glm::vec3 b, glm::vec3 color)
	: VAO{ 0 }, VBO{ 0 }, IBO{ 0 }, a{ a }, b{ b }, color{ color } {
	vertices.clear();
	indices.clear();

	vertices.push_back(a.x);
	vertices.push_back(a.y);
	vertices.push_back(a.z);

	vertices.push_back(b.x);
	vertices.push_back(b.y);
	vertices.push_back(b.z);

	indices.push_back(0);
	indices.push_back(1);
}

void Line::createMesh() {
	// Generate vertex array object and bind it
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Generate vertex buffer object and bind it
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(float), indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Line::renderMesh(GLenum renderMode) {
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	glDrawElements(renderMode, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, (void*)0);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Line::clearMesh() {
	if (VAO != 0)
		glDeleteVertexArrays(1, &VAO);

	if (VBO != 0)
		glDeleteBuffers(1, &VBO);

	if (IBO != 0)
		glDeleteBuffers(1, &IBO);
}