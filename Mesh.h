#pragma once

#include "Shader.h"
#include "Camera.h"
#include "Texture.h"

struct Vertex {
	glm::vec3 position;
	glm::vec2 texel;
	glm::vec3 normal;
	glm::vec3 tangent;

	Vertex(
		glm::vec3 position = glm::vec3(0.f), glm::vec2 texel = glm::vec2(0.f),
		glm::vec3 normal = glm::vec3(0.f), glm::vec3 tangent = glm::vec3(0.f)
	) : position{ position }, texel{ texel }, normal{ normal }, tangent{ tangent } {
	}
};

struct MeshMetaData {
	uint baseVertex;
	uint baseIndex;
	uint numIndices;
	uint materialIndex;

	MeshMetaData(uint baseVertex = 0, uint baseIndex = 0, uint numIndices = 0, uint materialIndex = 0)
		: baseVertex{ baseVertex }, baseIndex{ baseIndex }, numIndices{ numIndices }, materialIndex{ materialIndex } {
	}
};

class Mesh {
protected:
	static uint meshCount;
	uint objectID{ 0 };

	glm::vec3 color{ 1.f };
	glm::mat4 model{ 1.f };
	glm::mat4 outlineModel{ 1.f };

	std::vector<Vertex> vertices;
	std::vector<uint> indices;
	std::vector<MeshMetaData> renderData;

	uint indexOffset{ 0 }, vertexOffset{ 0 };
	GLfloat metallic{ 0.f }, roughness{ 0.f }, ao{ 0.f };

	GLuint VAO{ 0 };
	GLuint VBO{ 0 };
	GLuint IBO{ 0 };

	Texture* diffuseMap = nullptr;
	Texture* normalMap = nullptr;
	Texture* depthMap = nullptr;
	Texture* metallicMap = nullptr;
	Texture* roughnessMap = nullptr;

	bool useDiffuseMap{ false };
	bool useNormalMap{ false };
	bool strippedNormalMap{ false };
	bool useMaterialMap{ false };
	bool useEmissiveMap{ false };
	bool drawIndexed{ true };
	bool calcShadows{ false };
	bool enableSSAO{ false };

public:
	static std::vector<Mesh*> meshList;
	Mesh();

	virtual void setVertices(const std::vector<Vertex>& vertices) { this->vertices = vertices; }
	virtual void setIndices(const std::vector<uint>& indices) { this->indices = indices; }

	glm::mat4& getModelMatrix() { return this->model; }
	glm::vec3 getColor() const { return this->color; }
	uint getObjectID() const { return this->objectID; }
	float getMetallic() const { return this->metallic; }
	float getRoughness() const { return this->roughness; }
	float getAO() const { return this->ao; }
	bool isDrawIndexed() const { return this->drawIndexed; }

	bool getDiffuseMapBool() const { return this->useDiffuseMap; }
	bool getNormalMapBool() const { return this->useNormalMap; }
	bool getMaterialMapBool() const { return this->useMaterialMap; }
	bool getStrippedNormalBool() const { return this->strippedNormalMap; }

	void setDiffuseMap(Texture* materialMap) { this->diffuseMap = materialMap; }
	void setNormalMap(Texture* materialMap) { this->normalMap = materialMap; }
	void setMetallicMap(Texture* materialMap) { this->metallicMap = materialMap; }
	void setRoughnessMap(Texture* materialMap) { this->roughnessMap = materialMap; }

	void bindDiffuseMap() const { this->diffuseMap->useTexture(GL_TEXTURE0); }
	void bindNormalMap() const { this->normalMap->useTexture(GL_TEXTURE1); }
	void bindRoughnessMap() const { this->roughnessMap->useTexture(GL_TEXTURE2); }
	void bindMetallicMap() const { this->metallicMap->useTexture(GL_TEXTURE3); }

	void setModelMatrix(const glm::mat4& matrix) { this->model = matrix; }
	void setColor(glm::vec3 color) { this->color = color; }
	void setObjectID(int objectID) { this->objectID = objectID; }
	void setShadowBoolUniform(bool calcShadows) { this->calcShadows = calcShadows; }
	void setSSAOboolUniform(bool enableSSAO) { this->enableSSAO = enableSSAO; }

	void loadMesh(
		bool useDiffuseMap = false, bool drawIndexed = true,
		bool useNormalMap = false, bool useMaterialMap = false,
		bool useEmissiveMap = false, bool isStrippedNormal = false
	);

	void drawMesh(GLenum renderMode);
	void renderMesh(Shader& shader, GLenum renderMode = GL_TRIANGLES);

	void renderMeshWithOutline(Shader& shader, Shader& outlineShader, GLenum renderMode);

	void setMeshMaterial(float roughness, float metallic, float ao);

	void clearMesh();

	~Mesh() {
		this->clearMesh();
	}
};