#include "Model.h"

Model::Model(std::string fileName, std::string texFolderPath, aiTextureType diffuseMap,
	aiTextureType normalMap, aiTextureType metallicMap, bool isStrippedNormal)
	: Mesh(), texFolderPath{ texFolderPath }
{
	this->diffuseMaps.clear();
	this->normalMaps.clear();
	this->heightMaps.clear();
	this->metalnessMaps.clear();

	this->loadModel(fileName, diffuseMap, normalMap, metallicMap);
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
				}
				else {
					if (!maps[i]->loadDDSTexture()) {
						std::cerr << "Failed to load material at: " << texPath << std::endl;

						delete maps[i];
						maps[i] = nullptr;
					}
				}
			}
		}

		if (maps[i] == nullptr) {
			maps[i] = new Texture("Textures/prototype.png");
			maps[i]->loadTexture();
		}
	}
}

void Model::loadModel(std::string fileName, aiTextureType diffuseMap,
	aiTextureType normalMap, aiTextureType metallicMap) {
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
	this->_loadMaterialMap(scene, this->diffuseMaps, diffuseMap);
	this->_loadMaterialMap(scene, this->normalMaps, normalMap);
	this->_loadMaterialMap(scene, this->metalnessMaps, metallicMap);
	this->_loadMaterialMap(scene, this->heightMaps, aiTextureType_DIFFUSE_ROUGHNESS);
}

void Model::drawModel(GLenum renderMode) {
	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IBO);

	for (const MeshMetaData& mesh : this->renderData) {
		glDrawElementsBaseVertex(
			renderMode, mesh.numIndices, GL_UNSIGNED_INT, (void*)(mesh.baseIndex * sizeof(uint)), mesh.baseVertex
		);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Model::renderModel(PBRShader& shader, glm::vec3 cameraPosition, GLenum renderMode) {
	glUniform3fv(shader.getUniformCameraPosition(), 1, glm::value_ptr(cameraPosition));

	glUniform1ui(shader.getUniformTextureBool(), this->useDiffuseMap);
	glUniform1ui(shader.getUniformNormalMapBool(), this->useNormalMap);
	glUniform1ui(shader.getUniformUseMaterialMap(), this->useMaterialMap);
	glUniform1ui(shader.getUniformStrippedNormalBool(), this->strippedNormalMap);

	glUniformMatrix4fv(shader.getUniformModel(), 1, GL_FALSE, glm::value_ptr(this->model));
	glUniform3fv(shader.getUniformColor(), 1, glm::value_ptr(this->color));

	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IBO);

	for (size_t i = 0; i < this->renderData.size(); i++) {
		uint materialIndex = this->renderData[i].materialIndex;

		if (materialIndex < this->diffuseMaps.size() && diffuseMaps[materialIndex]) {
			this->diffuseMaps[materialIndex]->useTexture(GL_TEXTURE0);
		}

		if (materialIndex < this->normalMaps.size() && this->normalMaps[materialIndex]) {
			this->normalMaps[materialIndex]->useTexture(GL_TEXTURE1);
		}

		if (materialIndex < this->heightMaps.size() && this->heightMaps[materialIndex]) {
			this->heightMaps[materialIndex]->useTexture(GL_TEXTURE2);
		}

		if (materialIndex < this->metalnessMaps.size() && this->metalnessMaps[materialIndex]) {
			this->metalnessMaps[materialIndex]->useTexture(GL_TEXTURE3);
		}

		glDrawElementsBaseVertex(
			renderMode, this->renderData[i].numIndices, GL_UNSIGNED_INT,
			(void*)(sizeof(uint) * this->renderData[i].baseIndex), this->renderData[i].baseVertex
		);

		glActiveTexture(GL_TEXTURE0);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Model::clearModel() {
	for (size_t i = 0; i < this->diffuseMaps.size(); i++) {
		if (this->diffuseMaps[i]) {
			delete this->diffuseMaps[i];
			this->diffuseMaps[i] = nullptr;
		}
	}

	for (size_t i = 0; i < this->normalMaps.size(); i++) {
		if (this->normalMaps[i]) {
			delete this->normalMaps[i];
			this->normalMaps[i] = nullptr;
		}
	}

	for (size_t i = 0; i < this->heightMaps.size(); i++) {
		if (this->heightMaps[i]) {
			delete this->heightMaps[i];
			this->heightMaps[i] = nullptr;
		}
	}

	for (size_t i = 0; i < this->metalnessMaps.size(); i++) {
		if (this->metalnessMaps[i]) {
			delete this->metalnessMaps[i];
			this->metalnessMaps[i] = nullptr;
		}
	}
}