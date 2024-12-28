#include "UVSphere.h"

UVSphere::UVSphere(unsigned int stackCount, unsigned int sectorCount, float radius) 
	: Mesh(), stackCount{ stackCount }, sectorCount{ sectorCount }, radius{ radius } {

}

void UVSphere::addVertex(glm::vec3 position, glm::vec3 normal, glm::vec2 texel) {
	Vertex vertex{ position, texel, normal };

	vertices.push_back(vertex);
}

void UVSphere::addIndices(unsigned int a, unsigned int b, unsigned int c) {
	indices.push_back(a);
	indices.push_back(b);
	indices.push_back(c);
}

void UVSphere::generateSphere() {
	GLfloat x{}, y{}, z{}, xy{};
	GLfloat nx{}, ny{}, nz{}, lengthInv{ 1.f / radius };
	GLfloat u{}, v{};

	float sectorStep = 2.f * PI / sectorCount;
	float stackStep = PI / stackCount;
	float sectorAngle{}, stackAngle{};

	glm::vec3 position{}, normal{};
	glm::vec2 texel{};

	for (uint i = 0; i <= stackCount; i++) {
		stackAngle = (PI / 2.f) - (i * stackStep);
		xy = radius * cosf(stackAngle);
		z = radius * sinf(stackAngle);
		nz = z * lengthInv;

		for (uint j = 0; j <= sectorCount; j++) {
			sectorAngle = j * sectorStep;

			x = xy * cosf(sectorAngle);
			y = xy * sinf(sectorAngle);

			position = glm::vec3(x, y, z);

			nx = x * lengthInv;
			ny = y * lengthInv;
			
			normal = glm::vec3(nx, ny, nz);

			u = (float)j / sectorCount;
			v = (float)i / stackCount;

			texel = glm::vec2(u, v);
			addVertex(position, normal, texel);
		}
	}

	generateIndices();
}

void UVSphere::generateIndices() {
	int k1{}, k2{};

	for (int i = 0; i < (int)stackCount; i++) {
		k1 = i * (sectorCount + 1);
		k2 = k1 + sectorCount + 1;

		for (int j = 0; j < (int)sectorCount; j++, k1++, k2++) {
			if (i != 0)
				addIndices(k1, k2, k1 + 1);

			if (i != stackCount - 1)
				addIndices(k1 + 1, k2, k2 + 1);
		}
	}
}