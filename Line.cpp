#include "Line.h"

Line::Line(glm::vec3 a, glm::vec3 b, glm::vec3 color)
	: VAO{ 0 }, VBO{ 0 }, IBO{ 0 }, a{ a }, b{ b } {
	this->vertices.clear();
	this->indices.clear();

	this->vertices.push_back(a.x);
	this->vertices.push_back(a.y);
	this->vertices.push_back(a.z);

	this->vertices.push_back(b.x);
	this->vertices.push_back(b.y);
	this->vertices.push_back(b.z);

	this->indices.push_back(0);
	this->indices.push_back(1);

	this->color = color;
}

void Line::createMesh() {
	// Generate vertex array object and bind it
	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	// Generate vertex buffer object and bind it
	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(float), this->vertices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &this->IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(float), this->indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Line::renderMesh(GLenum renderMode) {
	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IBO);

	glDrawElements(renderMode, static_cast<GLsizei>(this->indices.size()), GL_UNSIGNED_INT, (void*)0);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Line::clearMesh() {
	if (this->VAO != 0)
		glDeleteVertexArrays(1, &this->VAO);

	if (this->VBO != 0)
		glDeleteBuffers(1, &this->VBO);

	if (this->IBO != 0)
		glDeleteBuffers(1, &this->IBO);
}