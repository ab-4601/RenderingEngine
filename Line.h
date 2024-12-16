#pragma once

#include "Core.h"

class Line {
private:
	GLuint VAO, VBO, IBO;

	std::vector<GLfloat> vertices;
	std::vector<unsigned int> indices;

	glm::vec3 a;
	glm::vec3 b;

	glm::vec3 color;

public:
	Line(glm::vec3 a = { 0.f, 0.f, 0.f }, glm::vec3 b = { 0.f, 0.f, 0.f }, glm::vec3 color = { 0.f, 0.f, 0.f });

	void createMesh();
	void renderMesh(GLenum renderMode = GL_LINES);
	void clearMesh();

	inline glm::vec3 getColor() const { return this->color; }

	~Line() {
		this->clearMesh();
	}
};