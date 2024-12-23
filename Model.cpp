#include "Model.h"

Model::Model(std::string fileName, std::string texFolderPath, aiTextureType diffuseMap,
	aiTextureType normalMap, aiTextureType metallicMap, aiTextureType emissiveMap,
	bool isStrippedNormal, bool hasEmissiveMaterial)
	: Mesh(), texFolderPath{ texFolderPath }, useEmissiveMap{ hasEmissiveMaterial }
{
	this->diffuseTextures.clear();
	this->normalTextures.clear();
	this->heightTextures.clear();
	this->metalnessTextures.clear();
	this->emissiveTextures.clear();

	this->loadModel(fileName, diffuseMap, normalMap, metallicMap, emissiveMap);
	this->loadMesh(true, true, true, true, isStrippedNormal);

	Mesh::meshList.pop_back();
}

void Model::_updateRenderData(const aiScene* scene) {
	this->renderData.resize(scene->mNumMeshes);

	for (size_t i = 0; i < this->renderData.size(); i++) {
		this->renderData[i].materialIndex = scene->mMeshes[i]->mMaterialIndex;
		this->renderData[i].numIndices = scene->mMeshes[i]->mNumFaces * 3;
		this->renderData[i].baseVertex = this->vertexOffset;
		this->renderData[i].baseIndex = this->indexOffset;

		this->vertexOffset += scene->mMeshes[i]->mNumVertices;
		this->indexOffset += this->renderData[i].numIndices;
	}

	this->vertexOffset = 0;
}

void Model::_loadNode(aiNode* node, const aiScene* const scene) {
	for (size_t i = 0; i < node->mNumMeshes; i++)
		this->_loadMesh(scene->mMeshes[node->mMeshes[i]], scene);

	for (size_t i = 0; i < node->mNumChildren; i++)
		this->_loadNode(node->mChildren[i], scene);
}

void Model::_loadMesh(aiMesh* mesh, const aiScene* const scene) {
	Vertex vertex{};

	for (size_t i = 0; i < mesh->mNumVertices; i++) {
		vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		vertex.texel = (mesh->mTextureCoords[0]) ? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y)
			: glm::vec2(0.f);
		vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		vertex.tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);

		this->vertices.push_back(vertex);
	}

	for (size_t i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (size_t j = 0; j < face.mNumIndices; j++) {
			this->indices.push_back(face.mIndices[j]);
		}
	}
}

void Model::_loadMaterialMap(const aiScene* const scene, std::vector<Texture*>& maps, aiTextureType textureType) const {
	maps.resize(scene->mNumMaterials);

	for (size_t i = 0; i < scene->mNumMaterials; i++) {
		aiMaterial* material = scene->mMaterials[i];
		maps[i] = nullptr;

		if (material->GetTextureCount(textureType)) {
			aiString path;

			if (material->GetTexture(textureType, 0, &path) == AI_SUCCESS) {
				size_t idx = std::string(path.data).rfind("\\");
				std::string filename = std::string(path.data).substr(idx + 1);

				size_t extIdx = filename.rfind(".");
				std::string extension = filename.substr(extIdx + 1);

				std::string texPath = this->texFolderPath + filename;

				maps[i] = new Texture(texPath);

				if (extension != "dds") {
					if (!maps[i]->loadTexture()) {
						std::cerr << "Failed to load material at: " << texPath << std::endl;

						delete maps[i];
						maps[i] = nullptr;
					}

					if (!maps[i]->makeBindless()) {
						std::cerr << "Failed to load texture to GPU memory: " << texPath << std::endl;
					}
				}
				else {
					if (!maps[i]->loadDDSTexture()) {
						std::cerr << "Failed to load material at: " << texPath << std::endl;

						delete maps[i];
						maps[i] = nullptr;
					}

					if (!maps[i]->makeBindless()) {
						std::cerr << "Failed to load texture to GPU memory: " << texPath << std::endl;
					}
				}
			}
		}

		if (maps[i] == nullptr) {
			maps[i] = new Texture("Textures/black.dds");
			maps[i]->loadDDSTexture();
			maps[i]->makeBindless();
		}
	}
}

void Model::loadModel(std::string fileName, aiTextureType diffuseMap,
	aiTextureType normalMap, aiTextureType metallicMap, aiTextureType emissiveMap) {
	if (fileName == "") {
		std::cout << "No file path specified" << std::endl;
		return;
	}

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(fileName, ASSIMP_LOAD_FLAGS);

	if (!scene) {
		std::cerr << fileName + " failed to load" << std::endl;
		std::cerr << importer.GetErrorString() << std::endl;
		return;
	}

	this->_updateRenderData(scene);
	this->_loadNode(scene->mRootNode, scene);
	this->_loadMaterialMap(scene, this->diffuseTextures, diffuseMap);
	this->_loadMaterialMap(scene, this->normalTextures, normalMap);
	this->_loadMaterialMap(scene, this->metalnessTextures, metallicMap);
	this->_loadMaterialMap(scene, this->heightTextures, aiTextureType_DIFFUSE_ROUGHNESS);

	if(this->useEmissiveMap)
		this->_loadMaterialMap(scene, this->emissiveTextures, emissiveMap);
}

void Model::drawModel(GLenum renderMode) {
	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IBO);

	for (const auto& mesh : this->renderData)
		glDrawElementsBaseVertex(
			renderMode, mesh.numIndices, GL_UNSIGNED_INT, (void*)(sizeof(uint) * mesh.baseIndex), mesh.baseVertex
		);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Model::renderModel(const PBRShader& shader, GLenum renderMode) {
	glUniform1ui(shader.getUniformTextureBool(), this->useDiffuseMap);
	glUniform1ui(shader.getUniformNormalMapBool(), this->useNormalMap);
	glUniform1ui(shader.getUniformUseMaterialMap(), this->useMaterialMap);
	glUniform1ui(shader.getUniformStrippedNormalBool(), this->strippedNormalMap);
	glUniform1ui(shader.getUniformUseEmissiveMap(), this->useEmissiveMap);

	glUniformMatrix4fv(shader.getUniformModel(), 1, GL_FALSE, glm::value_ptr(this->model));

	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IBO);

	for (const auto& mesh: this->renderData) {
		if (mesh.materialIndex < this->diffuseTextures.size() && diffuseTextures[mesh.materialIndex]) 
			this->diffuseTextures[mesh.materialIndex]->useTextureBindless(shader.getUniformDiffuseSampler());

		if (mesh.materialIndex < this->normalTextures.size() && this->normalTextures[mesh.materialIndex]) 
			this->normalTextures[mesh.materialIndex]->useTextureBindless(shader.getUniformNormalSampler());

		if (mesh.materialIndex < this->heightTextures.size() && this->heightTextures[mesh.materialIndex])
			this->heightTextures[mesh.materialIndex]->useTextureBindless(shader.getUniformDepthSampler());

		if (mesh.materialIndex < this->metalnessTextures.size() && this->metalnessTextures[mesh.materialIndex])
			this->metalnessTextures[mesh.materialIndex]->useTextureBindless(shader.getUniformMetallicSampler());

		if (mesh.materialIndex < this->emissiveTextures.size() && this->emissiveTextures[mesh.materialIndex]) 
			this->emissiveTextures[mesh.materialIndex]->useTextureBindless(shader.getUniformEmissiveSampler());

		glDrawElementsBaseVertex(
			renderMode, mesh.numIndices, GL_UNSIGNED_INT, (void*)(sizeof(uint) * mesh.baseIndex), mesh.baseVertex
		);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Model::clearModel() {
	for (size_t i = 0; i < this->diffuseTextures.size(); i++) {
		if (this->diffuseTextures[i]) {
			delete this->diffuseTextures[i];
			this->diffuseTextures[i] = nullptr;
		}
	}

	for (size_t i = 0; i < this->normalTextures.size(); i++) {
		if (this->normalTextures[i]) {
			delete this->normalTextures[i];
			this->normalTextures[i] = nullptr;
		}
	}

	for (size_t i = 0; i < this->heightTextures.size(); i++) {
		if (this->heightTextures[i]) {
			delete this->heightTextures[i];
			this->heightTextures[i] = nullptr;
		}
	}

	for (size_t i = 0; i < this->metalnessTextures.size(); i++) {
		if (this->metalnessTextures[i]) {
			delete this->metalnessTextures[i];
			this->metalnessTextures[i] = nullptr;
		}
	}
}