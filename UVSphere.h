#pragma once

#include "Core.h"
#include "Mesh.h"

class UVSphere : public Mesh {
private:
	unsigned int stackCount, sectorCount;
	float radius;

	void addVertex(glm::vec3 position, glm::vec3 normal, glm::vec2 texel);
	void addIndices(unsigned int a, unsigned int b, unsigned int c);

	void generateIndices();

public:
	UVSphere(unsigned int stackCount = 10, unsigned int sectorCount = 10, float radius = 1.f);

	void generateSphere();

	~UVSphere() = default;
};

