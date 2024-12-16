#pragma once

#include "Mesh.h"

class Icosphere : public Mesh {
private:
	std::vector<unsigned int> tempIndices;
	std::vector<GLfloat> tempVertices;

	void addVertex(float x, float y, float z);
	void addTempVertices(float x, float y, float z);
	void addTempIndices(unsigned int a, unsigned int b, unsigned int c);
	void addTexCoord(glm::vec2 uvCoord);

	glm::vec2 getTexCoord(glm::vec3 point);

public:
	Icosphere();

	void scale(float _scale) {
		for (auto& elem : this->vertices) {
			elem *= _scale;
		}
	}

	void _generateIcosohedron();
	void _subdivide(glm::vec3 a, glm::vec3 b, glm::vec3 c, unsigned int index1, unsigned int index2, unsigned int index3);
	void _generateTexCoords();
	void smoothSphere(int subdivisions);

	~Icosphere() {
		this->clearMesh();
	}
};

