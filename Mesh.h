#pragma once

#include "Core.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "PBRShader.h"

struct Vertex {
	glm::vec3 position;
	glm::vec2 texel;
	glm::vec3 normal;
	glm::vec3 tangent;

	Vertex(
		glm::vec3 position = glm::vec3(0.f), glm::vec2 texel = glm::vec2(0.f), 
		glm::vec3 normal = glm::vec3(0.f), glm::vec3 tangent = glm::vec3(0.f)
	) : position{ position }, texel{ texel }, normal{ normal }, tangent{ tangent } {}
};

class Mesh {
protected:
	static uint meshCount;
	uint objectID;

	glm::vec3 color;
	glm::mat4 model;
	glm::mat4 outlineModel;
	std::vector<Vertex> vertices;
	std::vector<uint> indices;

	GLfloat metallic{ 0.f }, roughness{ 0.f }, ao{ 0.f };

	GLuint VAO;
	GLuint VBO;
	GLuint IBO;

	Texture* diffuseMap = nullptr;
	Texture* normalMap = nullptr;
	Texture* depthMap = nullptr;
	Texture* metallicMap = nullptr;
	Texture* roughnessMap = nullptr;

	bool useDiffuseMap;
	bool useNormalMap;
	bool strippedNormalMap;
	bool useMaterialMap;
	bool drawIndexed;
	bool calcShadows;
	bool enableSSAO;

public:
	static std::vector<Mesh*> meshList;
	Mesh();

	virtual inline void setVertices(const std::vector<Vertex>& vertices) { this->vertices = vertices; }
	virtual inline void setIndices(const std::vector<uint>& indices) { this->indices = indices; }

	inline glm::mat4& getModelMatrix() { return this->model; }
	inline glm::vec3 getColor() const { return this->color; }
	inline uint getObjectID() const { return this->objectID; }
	inline float getMetallic() const { return this->metallic; }
	inline float getRoughness() const { return this->roughness; }
	inline float getAO() const { return this->ao; }
	inline bool isDrawIndexed() const { return this->drawIndexed; }

	inline bool getDiffuseMapBool() const { return this->useDiffuseMap; }
	inline bool getNormalMapBool() const { return this->useNormalMap; }
	inline bool getMaterialMapBool() const { return this->useMaterialMap; }

	inline void setDiffuseMap(Texture* materialMap) { this->diffuseMap = materialMap; }
	inline void setNormalMap(Texture* materialMap) { this->normalMap = materialMap; }
	inline void setMetallicMap(Texture* materialMap) { this->metallicMap = materialMap; }
	inline void setRoughnessMap(Texture* materialMap) { this->roughnessMap = materialMap; }

	inline void bindDiffuseMap() const { this->diffuseMap->useTexture(GL_TEXTURE0); }
	inline void bindNormalMap() const { this->normalMap->useTexture(GL_TEXTURE1); }
	inline void bindRoughnessMap() const { this->roughnessMap->useTexture(GL_TEXTURE2); }
	inline void bindMetallicMap() const { this->metallicMap->useTexture(GL_TEXTURE3); }

	inline void setModelMatrix(const glm::mat4& matrix) { this->model = matrix; }
	inline void setColor(glm::vec3 color) { this->color = color; }
	inline void setObjectID(int objectID) { this->objectID = objectID; }
	void inline setShadowBoolUniform(bool calcShadows) { this->calcShadows = calcShadows; }
	void inline setSSAOboolUniform(bool enableSSAO) { this->enableSSAO = enableSSAO; }

	void loadMesh(
		bool useDiffuseMap = false, bool drawIndexed = true,
		bool useNormalMap = false, bool useMaterialMap = false,
		bool isStrippedNormal = false
	);

	void drawMesh(GLenum renderMode);
	void renderMesh(PBRShader& shader, glm::vec3 cameraPosition, GLenum renderMode = GL_TRIANGLES);

	void renderMeshWithOutline(PBRShader& shader, Shader& outlineShader, GLenum renderMode, glm::vec3 cameraPosition);

	void setMeshMaterial(float roughness, float metallic, float ao);

	void clearMesh();

	~Mesh() {
		this->clearMesh();
	}
};