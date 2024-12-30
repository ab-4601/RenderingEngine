#pragma once

#include "Mesh.h"

class Icosphere : public Mesh {
private:
	std::vector<uint32_t> tempIndices;

	void addVertex(glm::vec3 position, glm::vec2 texel, glm::vec3 normal) {
		Vertex vertex{ position, texel, normal };
		this->vertices.push_back(vertex);
	}

	void addTempIndices(uint32_t a, uint32_t b, uint32_t c) {
		tempIndices.push_back(a);
		tempIndices.push_back(b);
		tempIndices.push_back(c);
	}

public:
	Icosphere();

	void _generateIcosahedron();
	void _subdivide(glm::vec3 a, glm::vec3 b, glm::vec3 c, uint32_t index1, uint32_t index2, uint32_t index3);
	void _generateTexCoords();
	void smoothSphere(int subdivisions);

	~Icosphere() {
		clearMesh();
	}
};

