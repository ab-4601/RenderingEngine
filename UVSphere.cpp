#include "UVSphere.h"

UVSphere::UVSphere(unsigned int stackCount, unsigned int sectorCount, float radius) 
	: Mesh(), stackCount{ stackCount }, sectorCount{ sectorCount }, radius{ radius } {

}

void UVSphere::addVertex(float x, float y, float z) {
	this->vertices.push_back(x);
	this->vertices.push_back(y);
	this->vertices.push_back(z);
}

void UVSphere::addNormal(float x, float y, float z) {
	this->normals.push_back(x);
	this->normals.push_back(y);
	this->normals.push_back(z);
}

void UVSphere::addTexel(float u, float v) {
	this->texCoords.push_back(u);
	this->texCoords.push_back(v);
}

void UVSphere::addIndices(unsigned int a, unsigned int b, unsigned int c) {
	this->indices.push_back(a);
	this->indices.push_back(b);
	this->indices.push_back(c);
}

void UVSphere::generateSphere() {
	GLfloat x{}, y{}, z{}, xy{};
	GLfloat nx{}, ny{}, nz{}, lengthInv{ 1.f / this->radius };
	GLfloat u{}, v{};

	float sectorStep = 2 * PI / this->sectorCount;
	float stackStep = PI / this->stackCount;
	float sectorAngle{}, stackAngle{};

	for (int i = 0; i <= this->stackCount; i++) {
		stackAngle = (PI / 2) - (i * stackStep);
		xy = this->radius * cosf(stackAngle);
		z = this->radius * sinf(stackAngle);
		nz = z * lengthInv;

		for (int j = 0; j <= this->sectorCount; j++) {
			sectorAngle = j * sectorStep;

			x = xy * cosf(sectorAngle);
			y = xy * sinf(sectorAngle);

			this->addVertex(x, y, z);

			nx = x * lengthInv;
			ny = y * lengthInv;
			
			this->addNormal(nx, ny, nz);

			u = (float)j / sectorCount;
			v = (float)i / stackCount;

			this->addTexel(u, v);
		}
	}

	this->generateIndices();
}

void UVSphere::generateIndices() {
	int k1{}, k2{};

	for (int i = 0; i < (int)this->stackCount; i++) {
		k1 = i * (sectorCount + 1);
		k2 = k1 + sectorCount + 1;

		for (int j = 0; j < (int)this->sectorCount; j++, k1++, k2++) {
			if (i != 0)
				this->addIndices(k1, k2, k1 + 1);

			if (i != stackCount - 1)
				this->addIndices(k1 + 1, k2, k2 + 1);
		}
	}
}