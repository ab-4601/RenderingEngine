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
	) : position{ position }, texel{ texel }, normal{ normal }, tangent{ tangent } { }
};

struct MeshMetaData {
	uint32_t baseVertex;
	uint32_t baseIndex;
	uint32_t numIndices;
	uint32_t materialIndex;

	MeshMetaData(uint32_t baseVertex = 0, uint32_t baseIndex = 0, uint32_t numIndices = 0, uint32_t materialIndex = 0)
		: baseVertex{ baseVertex }, baseIndex{ baseIndex }, numIndices{ numIndices }, materialIndex{ materialIndex } { }
};

struct DrawCommand {
	uint32_t indexCount;
	uint32_t instancedCount;
	uint32_t baseIndex;
	uint32_t baseVertex;
	uint32_t baseInstance;

	DrawCommand(uint32_t indexCount = 0, uint32_t instancedCount = 0, uint32_t baseIndex = 0,
		uint32_t baseVertex = 0, uint32_t baseInstance = 0)
		: indexCount{ indexCount }, instancedCount{ instancedCount }, baseIndex{ baseIndex },
		baseVertex{ baseVertex }, baseInstance{ baseInstance } { }
};

struct RenderData3D {
	int meshIndex;
	uint64_t diffuseMap;
	uint64_t normalMap;
	uint64_t metallicMap;
	uint64_t emissiveMap;

	RenderData3D(int meshIndex = 0, uint64_t diffuseMap = 0, uint64_t normalMap = 0,
		uint64_t metallicMap = 0, uint64_t emissiveMap = 0)
		: meshIndex{ meshIndex }, diffuseMap{ diffuseMap }, normalMap{ normalMap }, metallicMap{ metallicMap },
		emissiveMap{ emissiveMap } { }
};

class Mesh {
protected:
	static uint meshCount;
	uint objectID{ 0 };

	glm::vec3 color{ 1.f };
	glm::mat4 model{ 1.f };
	glm::mat4 outlineModel{ 1.f };

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<MeshMetaData> meshData;
	std::vector<DrawCommand> drawCommands;
	std::vector<RenderData3D> renderData;

	uint indexOffset{ 0 }, vertexOffset{ 0 };
	GLfloat metallic{ 0.f }, roughness{ 0.f }, ao{ 0.f };

	GLuint VAO{ 0 };
	GLuint VBO{ 0 };
	GLuint EBO{ 0 };
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
	Mesh();

	virtual void setVertices(const std::vector<Vertex>& vertices) { this->vertices = vertices; }
	virtual void setIndices(const std::vector<uint>& indices) { this->indices = indices; }

	glm::mat4& getModelMatrix() { return model; }
	glm::vec3 getColor() const { return color; }
	uint getObjectID() const { return objectID; }
	float getMetallic() const { return metallic; }
	float getRoughness() const { return roughness; }
	float getAO() const { return ao; }
	bool isDrawIndexed() const { return drawIndexed; }

	bool getDiffuseMapBool() const { return useDiffuseMap; }
	bool getNormalMapBool() const { return useNormalMap; }
	bool getMaterialMapBool() const { return useMaterialMap; }
	bool getStrippedNormalBool() const { return strippedNormalMap; }

	void setDiffuseMap(Texture* materialMap) { diffuseMap = materialMap; }
	void setNormalMap(Texture* materialMap) { normalMap = materialMap; }
	void setMetallicMap(Texture* materialMap) { metallicMap = materialMap; }
	void setRoughnessMap(Texture* materialMap) { roughnessMap = materialMap; }

	void bindDiffuseMap() const { diffuseMap->useTexture(GL_TEXTURE0); }
	void bindNormalMap() const { normalMap->useTexture(GL_TEXTURE1); }
	void bindRoughnessMap() const { roughnessMap->useTexture(GL_TEXTURE2); }
	void bindMetallicMap() const { metallicMap->useTexture(GL_TEXTURE3); }

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
		clearMesh();
	}
};