#include "Terrain.h"

Terrain::Terrain(GLuint rows, GLuint cols, int maxAmplitude, int maxPersistence,
	int maxFrequency, int frequencyDivisor, int persistenceDivisor,
	int maxRandomSeed, int maxOctaves)
	: Mesh(), rows{ rows }, cols{ cols }, scale{ 1.f }, maxAmplitude{ maxAmplitude }, maxPersistence{ maxPersistence },
	maxFrequency{ maxFrequency }, frequencyDivisor{ frequencyDivisor }, persistenceDivisor{ persistenceDivisor },
	maxRandomSeed{ maxRandomSeed }, maxOctaves{ maxOctaves } 
{
	this->vertexCoords.clear();
	this->noise.setOctaves(5);
}

void Terrain::generateIndices() {
	int num_of_points = this->rows * this->cols;

	for (int i = 0; i < num_of_points; i++)
	{
		if ((i + 1) % (int)this->cols == 0)
			continue;

		if (i + this->cols >= num_of_points) {
			this->indices.push_back(static_cast<unsigned int>(i));
			this->indices.push_back(static_cast<unsigned int>(i - (int)this->cols));
			this->indices.push_back(static_cast<unsigned int>(i + 1));
		}
		else if (i - (int)this->cols < 0) {
			this->indices.push_back(static_cast<unsigned int>(i));
			this->indices.push_back(static_cast<unsigned int>(i + 1));
			this->indices.push_back(static_cast<unsigned int>(i + 1 + this->cols));
		}
		else {
			this->indices.push_back(static_cast<unsigned int>(i));
			this->indices.push_back(static_cast<unsigned int>(i + 1));
			this->indices.push_back(static_cast<unsigned int>(i + 1 + this->cols));

			this->indices.push_back(static_cast<unsigned int>(i));
			this->indices.push_back(static_cast<unsigned int>(i - (int)this->cols));
			this->indices.push_back(static_cast<unsigned int>(i + 1));
		}
	}
}

void Terrain::generateTexCoords() {
	for (int i = 0; i < this->rows; i++) {
		for (int j = 0; j < this->cols; j++) {
			this->texCoords.push_back((float)j / 10);
			this->texCoords.push_back((float)i / 10);
		}
	}
}

void Terrain::generateVertexNormals() {
	int numOfPoints = this->rows * this->cols;
	int currRow = 1;

	glm::vec3 currPoint{}, a{}, b{};
	glm::vec3 normal;

	if (noise.getAmplitude() == 0.f) {
		normal = glm::vec3(0.f, 1.f, 0.f);
		for (int i = 0; i < numOfPoints; i++) {
			this->addNormals(normal);
		}
	}
	else {
		for (int i = 0; i < numOfPoints; i++) {
			currPoint = this->vertexCoords.at(i);

			if (currRow == this->rows) {
				if ((i + 1) % this->cols == 0) {
					a = this->vertexCoords.at(i - 1);
					b = this->vertexCoords.at(i - this->cols);
				}
				else {
					a = this->vertexCoords.at(i - this->cols);
					b = this->vertexCoords.at(i + 1);
				}
			}
			else {
				if ((i + 1) % this->cols == 0) {
					a = this->vertexCoords.at(i + this->cols);
					b = this->vertexCoords.at(i - 1);
				}
				else {
					a = this->vertexCoords.at(i + 1);
					b = this->vertexCoords.at(i + 1 + this->cols);
				}
			}

			if ((i + 1) % this->cols == 0)
				++currRow;

			normal = glm::cross(a - currPoint, b - currPoint);
			normal *= this->scale;

			this->addNormals(normal);
		}
	}
}

void Terrain::generateHeightMaps(int noOfMaps) {
	float height{};
	std::vector<float> currHeightMap{};

	while (noOfMaps > 0) {
		currHeightMap.clear();

		this->noise.setAmplitude(rand() % this->maxAmplitude);
		this->noise.setRandomSeed(rand() % this->maxRandomSeed);
		this->noise.setFrequency((float)(rand() % this->maxFrequency) / this->frequencyDivisor);
		this->noise.setPersistence((float)(rand() % this->maxPersistence) / this->persistenceDivisor);

		for (int r = 0; r < (int)this->rows; r++) {
			for (int c = 0; c < (int)this->cols; c++) {
				height = this->noise.getHeight(r, c);

				currHeightMap.push_back(height);
			}
		}

		this->heightMaps.push_back(currHeightMap);

		--noOfMaps;
	}
}

void Terrain::generateTerrain(float scale) {
	this->scale = scale;
	float height = 0.f;
	
	std::vector<float> finalHeightMap{};

	for (int i = 0; i < this->rows * this->cols; i++) {
		height = 0.f;
		for (size_t j = 0; j < this->heightMaps.size(); j++) {
			height += this->heightMaps.at(j).at(i);
		}

		finalHeightMap.push_back(height);
	}

	int index = 0;

	if (this->heightMaps.size() == 0) {
		for (int r = -(int)this->rows / 2; r < (int)this->rows / 2; r++) {
			for (int c = -(int)this->cols / 2; c < (int)this->cols / 2; c++) {
				glm::vec3 vertex(r * this->scale, 0, c * this->scale);

				this->addVertex(vertex);
				this->vertexCoords.push_back(vertex);
			}
		}
	}
	else {
		for (int r = -(int)this->rows / 2; r < (int)this->rows / 2; r++) {
			for (int c = -(int)this->cols / 2; c < (int)this->cols / 2; c++) {
				glm::vec3 vertex(r * this->scale, finalHeightMap.at(index++), c * this->scale);

				this->addVertex(vertex);
				this->vertexCoords.push_back(vertex);
			}
		}
	}

	this->generateIndices();
	this->generateVertexNormals();
	this->generateTexCoords();
}