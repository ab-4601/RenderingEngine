#pragma once

#include "Line.h"
#include "Shader.h"
#include "Camera.h"

class CoordinateSystem {
private:
	std::vector<Line*> axes;

	Shader shader{ "coord.vert", "coord.frag" };
	glm::mat4 model = glm::mat4(1.f);

public:
	CoordinateSystem();

	void createCoordinateSystem();
	void drawCoordinateSystem(GLint windowHeight, GLint windowWidth, GLint bufferWidth,
		GLint bufferHeight, const Camera& camera);

	~CoordinateSystem();
};

