#pragma once

#include "Mesh.h"

class Icosphere : public Mesh {
private:
	std::vector<unsigned int> tempIndices;

	void addVertex(glm::vec3 position, glm::vec2 texel, glm::vec3 normal) {
		Vertex vertex{ position, texel, normal };
		this->vertices.push_back(vertex);
	}

	void addTempIndices(unsigned int a, unsigned int b, unsigned int c) {
		tempIndices.push_back(a);
		tempIndices.push_back(b);
		tempIndices.push_back(c);
	}

public:
	Icosphere();

	void _generateIcosahedron();
	void _subdivide(glm::vec3 a, glm::vec3 b, glm::vec3 c, unsigned int index1, unsigned int index2, unsigned int index3);
	void _generateTexCoords();
	void smoothSphere(int subdivisions);

	~Icosphere() {
		clearMesh();
	}
};

