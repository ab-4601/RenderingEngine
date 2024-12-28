#include "Terrain.h"

Terrain::Terrain(int rows, int cols, int maxAmplitude, int maxPersistence,
	int maxFrequency, int frequencyDivisor, int persistenceDivisor,
	int maxRandomSeed, int maxOctaves)
	: Mesh(), rows{ rows }, cols{ cols }, scale{ 1.f }, maxAmplitude{ maxAmplitude }, maxPersistence{ maxPersistence },
	maxFrequency{ maxFrequency }, frequencyDivisor{ frequencyDivisor }, persistenceDivisor{ persistenceDivisor },
	maxRandomSeed{ maxRandomSeed }, maxOctaves{ maxOctaves } 
{
	vertexCoords.clear();
	noise.setOctaves(5);
}

void Terrain::generateIndices() {
	int num_of_points = rows * cols;

	for (int i = 0; i < num_of_points; i++)
	{
		if ((i + 1) % cols == 0)
			continue;

		if (i + cols >= num_of_points) {
			indices.push_back(static_cast<uint>(i));
			indices.push_back(static_cast<uint>(i - cols));
			indices.push_back(static_cast<uint>(i + 1));
		}
		else if (i - cols < 0) {
			indices.push_back(static_cast<uint>(i));
			indices.push_back(static_cast<uint>(i + 1));
			indices.push_back(static_cast<uint>(i + 1 + cols));
		}
		else {
			indices.push_back(static_cast<uint>(i));
			indices.push_back(static_cast<uint>(i + 1));
			indices.push_back(static_cast<uint>(i + 1 + cols));

			indices.push_back(static_cast<uint>(i));
			indices.push_back(static_cast<uint>(i - cols));
			indices.push_back(static_cast<uint>(i + 1));
		}
	}
}

void Terrain::generateTexCoords() {
	for (float i = 0.f; i < (float)rows; i++) {
		for (float j = 0.f; j < (float)cols; j++) {
			texCoords.push_back(j / 10);
			texCoords.push_back(i / 10);
		}
	}

	int index = 0;

	for (Vertex& vertex : vertices) {
		vertex.texel = glm::vec2(texCoords[index], texCoords[index + 1]);
		index += 2;
	}
}

void Terrain::generateVertexNormals() {
	int numOfPoints = rows * cols;
	int currRow = 1;

	glm::vec3 currPoint{}, a{}, b{};
	glm::vec3 normal;

	if (noise.getAmplitude() == 0.f) {
		normal = glm::vec3(0.f, 1.f, 0.f);
		for (int i = 0; i < numOfPoints; i++)
			vertices[i].normal = normal;
	}
	else {
		for (int i = 0; i < numOfPoints; i++) {
			currPoint = vertices.at(i).position;

			if (currRow == rows) {
				if ((i + 1) % cols == 0) {
					a = vertices.at(i - 1).position;
					b = vertices.at(i - cols).position;
				}
				else {
					a = vertices.at(i - cols).position;
					b = vertices.at(i + 1).position;
				}
			}
			else {
				if ((i + 1) % cols == 0) {
					a = vertices.at(i + cols).position;
					b = vertices.at(i - 1).position;
				}
				else {
					a = vertices.at(i + 1).position;
					b = vertices.at(i + 1 + cols).position;
				}
			}

			if ((i + 1) % cols == 0)
				++currRow;

			normal = glm::cross(a - currPoint, b - currPoint);
			normal = glm::normalize(normal);

			vertices[i].normal = normal;
		}
	}
}

void Terrain::generateHeightMaps(int noOfMaps) {
	float height{};
	std::vector<float> currHeightMap{};

	while (noOfMaps > 0) {
		currHeightMap.clear();

		noise.setAmplitude(float(rand() % maxAmplitude));
		noise.setRandomSeed(rand() % maxRandomSeed);
		noise.setFrequency((float)(rand() % maxFrequency) / frequencyDivisor);
		noise.setPersistence((float)(rand() % maxPersistence) / persistenceDivisor);

		for (float r = 0; r < (float)rows; r++) {
			for (float c = 0; c < (float)cols; c++) {
				height = noise.getHeight(r, c);

				currHeightMap.push_back(height);
			}
		}

		heightMaps.push_back(currHeightMap);

		--noOfMaps;
	}
}

void Terrain::generateTerrain(float scale) {
	scale = scale;
	float height = 0.f;
	
	std::vector<float> finalHeightMap{};

	for (int i = 0; i < rows * cols; i++) {
		height = 0.f;
		for (size_t j = 0; j < heightMaps.size(); j++) {
			height += heightMaps.at(j).at(i);
		}

		finalHeightMap.push_back(height);
	}

	int index = 0;

	if (heightMaps.size() == 0) {
		for (int r = -rows / 2; r < rows / 2; r++) {
			for (int c = -cols / 2; c < cols / 2; c++) {
				glm::vec3 vertex(r * scale, 0, c * scale);

				vertices.push_back(Vertex{ vertex });
			}
		}
	}
	else {
		for (int r = -rows / 2; r < rows / 2; r++) {
			for (int c = -cols / 2; c < cols / 2; c++) {
				glm::vec3 vertex(r * scale, finalHeightMap.at(index++), c * scale);

				vertices.push_back(Vertex{ vertex });
			}
		}
	}

	generateIndices();
	generateVertexNormals();
	generateTexCoords();
}