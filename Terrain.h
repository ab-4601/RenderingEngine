#pragma once

#include "Mesh.h"
#include "PerlinNoise.h"

class Terrain : public Mesh {
private:
	GLuint rows, cols;
	float scale;

	PerlinNoise noise;

	int maxAmplitude, maxPersistence, maxFrequency;
	int frequencyDivisor, persistenceDivisor;
	int maxRandomSeed, maxOctaves;

	std::vector<glm::vec3> vertexCoords;
	std::vector<std::vector<float>> heightMaps;
	std::vector<float> texCoords;

	void generateIndices();
	void generateTexCoords();

public:
	Terrain(GLuint rows = 0, GLuint cols = 0, int maxAmplitude = 50, int maxPersistence = 10,
		int maxFrequency = 10, int frequencyDivisor = 100, int persistenceDivisor = 100, 
		int maxRandomSeed = 10, int maxOctaves = 10);

	void generateHeightMaps(int noOfMaps);
	void generateTerrain(float scale = 1.f);

	inline void setScale(float scale) { this->scale = scale; }

	void generateVertexNormals();

	~Terrain() = default;
};