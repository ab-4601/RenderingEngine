#pragma once

#include "Core.h"
#include "Mesh.h"
#include "Texture.h"

#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_FlipWindingOrder | aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_OptimizeMeshes)

class Model {
private:
	std::string texFolderPath;
	std::vector<Mesh*> meshList;
	std::vector<Texture*> diffuseMaps;
	std::vector<Texture*> normalMaps;
	std::vector<Texture*> heightMaps;
	std::vector<Texture*> metalnessMaps;
	std::vector<unsigned int> meshToTex;

	std::vector<GLfloat> vertices;
	std::vector<uint> indices;

	GLuint VAO{ 0 }, VBO{ 0 }, IBO{ 0 };

	void _loadNode(aiNode* node, const aiScene* const scene, bool isStrippedNormal);
	void _loadMesh(aiMesh* mesh, const aiScene* const scene, bool isStrippedNormal);
	void _loadMaterialMap(const aiScene* const scene, std::vector<Texture*>& maps, aiTextureType textureType);
	void createModel();

public:
	Model(std::string fileName = "", std::string texFolderPath = "",
		aiTextureType diffuseMap = aiTextureType_DIFFUSE,
		aiTextureType normalMap = aiTextureType_NORMALS,
		aiTextureType metallicMap = aiTextureType_METALNESS,
		bool isStrippedNormal = false
	);

	void loadModel(std::string fileName, aiTextureType diffuseMap,
		aiTextureType normalMap, aiTextureType metallicMap, bool isStrippedNormal);

	void renderModel(PBRShader& shader, glm::vec3 cameraPosition);

	void drawModel(GLenum renderMode);

	void clearModel();

	~Model() {
		this->clearModel();
	}
};