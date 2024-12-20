#include "Mesh.h"

Mesh::Mesh() {
	this->meshList.push_back(this);
	this->objectID = this->meshCount++;

	this->vertices.clear();
	this->indices.clear();
	this->renderData.clear();
}

void Mesh::loadMesh(bool useDiffuseMap, bool drawIndexed, bool useNormalMap, bool useMaterialMap, bool isStrippedNormal) {
	this->useDiffuseMap = useDiffuseMap;
	this->useNormalMap = useNormalMap;
	this->useMaterialMap = useMaterialMap;
	this->drawIndexed = drawIndexed;

	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), this->vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texel));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

	if (this->drawIndexed == true) {
		glGenBuffers(1, &this->IBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(uint), this->indices.data(), GL_STATIC_DRAW);
	}

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
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IBO);

		glDrawElements(renderMode, static_cast<GLsizei>(this->indices.size()), GL_UNSIGNED_INT, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	else {
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

		glDrawArrays(renderMode, 0, static_cast<GLsizei>(this->vertices.size()));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	glBindVertexArray(0);
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