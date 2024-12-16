#pragma once

#include "Core.h"
#include "Camera.h"
#include "Shader.h"

class Grid {
private:
	GLfloat vertices[12]{
		-1.f,  0.f, -1.f,
		 1.f,  0.f, -1.f,
		 1.f,  0.f,  1.f,
		-1.f,  0.f,  1.f
	};

	unsigned int indices[6]{
		0, 2, 1,
		2, 0, 3
	};

	GLuint VAO, VBO, IBO;
	Shader shader{ "grid.vert", "grid.frag" };

public:
	Grid();

	void renderGrid(glm::vec3 cameraPosition);

	~Grid();
};

