#pragma once

#include "Core.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "PBRShader.h"

struct Vertex {
	glm::vec3 position{ 0.f };
	glm::vec3 normal{ 0.f };
	glm::vec3 tangent{ 0.f };
	glm::vec3 biTangent{ 0.f };
	glm::vec2 texel{ 0.f };
};

class Mesh {
protected:
	static uint meshCount;
	uint objectID;

	glm::vec3 color;
	glm::mat4 model;
	glm::mat4 outlineModel;
	std::vector<GLfloat> vertices;
	std::vector<uint> indices;
	std::vector<GLfloat> texCoords;
	std::vector<GLfloat> normals;

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

	virtual inline void setVertices(const std::vector<GLfloat>& vertices) { this->vertices = vertices; }
	virtual inline void setIndices(const std::vector<uint>& indices) { this->indices = indices; }
	virtual inline void setTexCoords(const std::vector<GLfloat>& texCoords) { this->texCoords = texCoords; }
	virtual inline void setNormals(const std::vector<GLfloat>& normals) { this->normals = normals; }

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

	void createUnindexedMesh();
	void createUnindexedTexturedMesh();
	void createFlatMesh();
	void createTexturedMesh();
	void createMeshWithNormals();
	void createMesh();
	void createModel(bool isStrippedNormal);

	void drawMesh(GLenum renderMode);
	void renderMesh(PBRShader& shader, glm::vec3 cameraPosition, GLenum renderMode = GL_TRIANGLES);

	void renderMeshWithOutline(PBRShader& shader, Shader& outlineShader, GLenum renderMode, glm::vec3 cameraPosition);

	void setMeshMaterial(float roughness, float metallic, float ao);

	void clearMesh();

	~Mesh() {
		this->clearMesh();
	}
};