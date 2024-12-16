#include "Mesh.h"

Mesh::Mesh()
	: VAO{ 0 }, VBO{ 0 }, IBO{ 0 }, color{ 1.f, 1.f, 1.f }, model{ glm::mat4(1.f) }, objectID{ 0 }, enableSSAO{ false },
	useDiffuseMap{ false }, drawIndexed{ false }, useNormalMap{ false }, calcShadows{ false }, useMaterialMap{ false },
	strippedNormalMap{ false }
{
	meshList.push_back(this);
	this->objectID = this->meshCount++;

	this->vertices.clear();
	this->indices.clear();
	this->texCoords.clear();
	this->normals.clear();

	this->outlineModel = glm::scale(this->model, glm::vec3(1.1f, 1.1f, 1.1f));
}

void Mesh::createUnindexedMesh() {
	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(float), this->vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)0);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 3));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::createUnindexedTexturedMesh() {
	this->useDiffuseMap = true;

	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(float), this->vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float) * 3));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::createFlatMesh() {
	this->drawIndexed = true;

	// Generate vertex array object and bind it
	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	// Generate vertex buffer object and bind it
	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(float), this->vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// Generate index buffer object and bind it
	glGenBuffers(1, &this->IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(uint), this->indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::createTexturedMesh() {
	std::vector<GLfloat> tmpVertices = this->vertices;
	this->vertices.clear();

	this->useDiffuseMap = true;
	this->drawIndexed = true;

	for (size_t i = 0, j = 0; i < tmpVertices.size(); i++) {
		if (i % 3 == 0 && i != 0) {
			this->vertices.push_back(this->texCoords.at(j));
			this->vertices.push_back(this->texCoords.at(j + 1));

			j += 2;
		}

		this->vertices.push_back(tmpVertices.at(i));
	}

	tmpVertices.clear();

	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	glGenBuffers(1, &this->IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(uint), this->indices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(float), this->vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::createMeshWithNormals() {
	std::vector<GLfloat> tmpVertices = this->vertices;
	this->vertices.clear();

	this->drawIndexed = true;

	for (size_t i = 0; i < tmpVertices.size(); i += 3) {
		this->vertices.push_back(tmpVertices.at(i));
		this->vertices.push_back(tmpVertices.at(i + 1));
		this->vertices.push_back(tmpVertices.at(i + 2));

		this->vertices.push_back(this->normals.at(i));
		this->vertices.push_back(this->normals.at(i + 1));
		this->vertices.push_back(this->normals.at(i + 2));
	}

	tmpVertices.clear();

	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(float), this->vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &this->IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(uint), this->indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::createMesh() {
	this->useDiffuseMap = true;
	this->drawIndexed = true;

	if(this->normals.size() != 0 && this->texCoords.size() != 0) {
		std::vector<GLfloat> tmpVertices = this->vertices;
		this->vertices.clear();

		for (size_t i = 0, j = 0; i < tmpVertices.size(); i += 3) {
			this->vertices.push_back(tmpVertices.at(i));
			this->vertices.push_back(tmpVertices.at(i + 1));
			this->vertices.push_back(tmpVertices.at(i + 2));

			this->vertices.push_back(this->texCoords.at(j++));
			this->vertices.push_back(this->texCoords.at(j++));

			this->vertices.push_back(this->normals.at(i));
			this->vertices.push_back(this->normals.at(i + 1));
			this->vertices.push_back(this->normals.at(i + 2));
		}

		tmpVertices.clear();
	}

	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(float), this->vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 5));
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &this->IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(uint), this->indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::createModel(bool isStrippedNormal) {
	this->useDiffuseMap = true;
	this->drawIndexed = true;
	this->useNormalMap = true;
	this->useMaterialMap = true;
	this->strippedNormalMap = isStrippedNormal;

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

void Mesh::setMeshMaterial(float roughness, float metallic, float ao) {
	this->roughness = roughness;
	this->metallic = metallic;
	this->ao = ao;
}

void Mesh::drawMesh(GLenum renderMode) {
	glBindVertexArray(this->VAO);

	if (this->drawIndexed) {
		glDrawElements(renderMode, static_cast<GLsizei>(this->indices.size()), GL_UNSIGNED_INT, this->indices.data());
	}
	else {
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

		glDrawArrays(renderMode, 0, static_cast<GLsizei>(this->vertices.size()));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void Mesh::renderMesh(PBRShader& shader, glm::vec3 cameraPosition, GLenum renderMode)
{
	glUniform3fv(shader.getUniformCameraPosition(), 1, glm::value_ptr(cameraPosition));
	glUniform3fv(shader.getUniformAlbedo(), 1, glm::value_ptr(this->color));
	glUniform1f(shader.getUniformAo(), this->ao);
	glUniform1f(shader.getUniformMetallic(), this->metallic);
	glUniform1f(shader.getUniformRoughness(), this->roughness);

	glUniform1ui(shader.getUniformTextureBool(), this->useDiffuseMap);
	glUniform1ui(shader.getUniformNormalMapBool(), this->useNormalMap);
	glUniform1ui(shader.getUniformUseMaterialMap(), this->useMaterialMap);
	glUniform1ui(shader.getUniformStrippedNormalBool(), this->strippedNormalMap);

	glUniformMatrix4fv(shader.getUniformModel(), 1, GL_FALSE, glm::value_ptr(this->model));
	glUniform3fv(shader.getUniformColor(), 1, glm::value_ptr(this->color));

	this->drawMesh(renderMode);
}

void Mesh::renderMeshWithOutline(PBRShader& shader, Shader& outlineShader, GLenum renderMode, glm::vec3 cameraPosition)
{
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);

	this->renderMesh(shader, cameraPosition, renderMode);

	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	//glDisable(GL_DEPTH_TEST);

	outlineShader.useShader();

	outlineModel = glm::scale(this->model, glm::vec3(1.05f, 1.05f, 1.05f));
	outlineShader.setMat4("model", this->outlineModel);

	this->drawMesh(GL_TRIANGLES);

	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glDisable(GL_STENCIL_TEST);
	//glEnable(GL_DEPTH_TEST);

	outlineShader.endShader();

	glUseProgram(shader.getProgramID());
}

void Mesh::clearMesh() {
	if (this->IBO != 0) {
		glDeleteBuffers(1, &this->IBO);
		this->IBO = 0;
	}

	if (this->VBO != 0) {
		glDeleteBuffers(1, &this->VBO);
		this->VBO = 0;
	}

	if (this->VAO != 0) {
		glDeleteVertexArrays(1, &this->VAO);
		this->VAO = 0;
	}
}