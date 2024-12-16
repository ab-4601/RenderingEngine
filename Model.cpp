#include "Model.h"

Model::Model(std::string fileName, std::string texFolderPath, aiTextureType diffuseMap,
	aiTextureType normalMap, aiTextureType metallicMap, bool isStrippedNormal)
	: texFolderPath{ texFolderPath }
{
	this->meshList.clear();
	this->meshToTex.clear();
	this->diffuseMaps.clear();
	this->heightMaps.clear();

	this->loadModel(fileName, diffuseMap, normalMap, metallicMap, isStrippedNormal);
}

void Model::createModel() {
	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(float), this->vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (void*)(sizeof(float) * 5));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (void*)(sizeof(float) * 8));
	glEnableVertexAttribArray(3);

	glGenBuffers(1, &this->IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(uint), this->indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Model::_loadNode(aiNode* node, const aiScene* const scene, bool isStrippedNormal) {
	for (size_t i = 0; i < node->mNumMeshes; i++)
		this->_loadMesh(scene->mMeshes[node->mMeshes[i]], scene, isStrippedNormal);

	for (size_t i = 0; i < node->mNumChildren; i++)
		this->_loadNode(node->mChildren[i], scene, isStrippedNormal);
}

void Model::_loadMesh(aiMesh* mesh, const aiScene* const scene, bool isStrippedNormal) {
	this->vertices.clear();
	this->indices.clear();

	for (size_t i = 0; i < mesh->mNumVertices; i++) {
		this->vertices.insert(this->vertices.end(), { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z });
		
		if (mesh->mTextureCoords[0])
			this->vertices.insert(this->vertices.end(), { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y });
		else
			this->vertices.insert(this->vertices.end(), { 0.f, 0.f });

		this->vertices.insert(this->vertices.end(), { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z });

		this->vertices.insert(this->vertices.end(), { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z });
	}

	for (size_t i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (size_t j = 0; j < face.mNumIndices; j++) {
			this->indices.push_back(face.mIndices[j]);
		}
	}

	Mesh* currMesh = new Mesh();
	currMesh->setObjectID(-1);
	currMesh->setVertices(this->vertices);
	currMesh->setIndices(this->indices);
	currMesh->createModel(isStrippedNormal);

	this->meshList.push_back(currMesh);
	this->meshToTex.push_back(mesh->mMaterialIndex);
}

void Model::_loadMaterialMap(const aiScene* const scene, std::vector<Texture*>& maps, aiTextureType textureType) {
	maps.resize(scene->mNumMaterials);

	for (size_t i = 0; i < scene->mNumMaterials; i++) {
		aiMaterial* material = scene->mMaterials[i];
		maps[i] = nullptr;

		if (material->GetTextureCount(textureType)) {
			aiString path;

			if (material->GetTexture(textureType, 0, &path) == AI_SUCCESS) {
				int idx = std::string(path.data).rfind("\\");
				std::string filename = std::string(path.data).substr(idx + 1);

				int extIdx = filename.rfind(".");
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
	aiTextureType normalMap, aiTextureType metallicMap, bool isStrippedNormal) {
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

	this->_loadNode(scene->mRootNode, scene, isStrippedNormal);
	this->_loadMaterialMap(scene, this->diffuseMaps, diffuseMap);
	this->_loadMaterialMap(scene, this->normalMaps, normalMap);
	this->_loadMaterialMap(scene, this->metalnessMaps, metallicMap);
	this->_loadMaterialMap(scene, this->heightMaps, aiTextureType_DIFFUSE_ROUGHNESS);
}

void Model::drawModel(GLenum renderMode) {
	for (size_t i = 0; i < this->meshList.size(); i++) {
		this->meshList[i]->drawMesh(renderMode);
	}
}

void Model::renderModel(PBRShader& shader, glm::vec3 cameraPosition) {
	for (size_t i = 0; i < this->meshList.size(); i++) {
		uint materialIndex = this->meshToTex[i];

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

		this->meshList[i]->renderMesh(shader, cameraPosition, GL_TRIANGLES);

		glActiveTexture(GL_TEXTURE0);
	}
}

void Model::clearModel() {
	for (size_t i = 0; i < this->meshList.size(); i++) {
		if (this->meshList[i]) {
			delete this->meshList[i];
			this->meshList[i] = nullptr;
		}
	}

	for (size_t i = 0; i < this->diffuseMaps.size(); i++) {
		if (this->diffuseMaps[i]) {
			delete this->diffuseMaps[i];
			this->diffuseMaps[i] = nullptr;
		}
	}

	for (size_t i = 0; i < this->diffuseMaps.size(); i++) {
		if (this->normalMaps[i]) {
			delete this->normalMaps[i];
			this->normalMaps[i] = nullptr;
		}
	}
}