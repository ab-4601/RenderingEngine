#pragma once

#include "Mesh.h"

#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_FlipWindingOrder | aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_OptimizeMeshes)

class Model : public Mesh {
protected:
	std::string texFolderPath;
	std::vector<Texture*> diffuseTextures;
	std::vector<Texture*> normalTextures;
	std::vector<Texture*> heightTextures;
	std::vector<Texture*> metalnessTextures;
	std::vector<Texture*> emissiveTextures;

	GLuint RSBO{ 0 };

	void _loadNode(aiNode* node, const aiScene* const scene);
	void _loadMesh(aiMesh* mesh, const aiScene* const scene);
	void _updateMeshData(const aiScene* node);
	void _updateRenderData();
	void _updateIndirectBuffer();
	void _loadMaterialMap(const aiScene* const scene, std::vector<Texture*>& maps, aiTextureType textureType) const;

public:
	Model(std::string fileName = "", std::string texFolderPath = "",
		aiTextureType diffuseMap = aiTextureType_DIFFUSE,
		aiTextureType normalMap = aiTextureType_NORMALS,
		aiTextureType metallicMap = aiTextureType_METALNESS,
		aiTextureType emissiveMap = aiTextureType_EMISSIVE,
		bool isStrippedNormal = false, bool hasEmissiveMaterial = false
	);

	void loadModel(std::string fileName, aiTextureType diffuseMap,
		aiTextureType normalMap, aiTextureType metallicMap, aiTextureType emissiveMap);

	void drawModel(GLenum renderMode = GL_TRIANGLES);
	void renderModel(Shader& shader, GLenum renderMode = GL_TRIANGLES);
	void renderModelWithOutline(Shader& shader, Shader& outlineShader, GLenum renderMode = GL_TRIANGLES);

	void clearModel();

	~Model() {
		clearModel();
	}
};