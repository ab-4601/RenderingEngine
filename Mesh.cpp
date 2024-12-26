#include "Mesh.h"

Mesh::Mesh() {
	this->meshList.push_back(this);
	this->objectID = this->meshCount++;

	this->vertices.clear();
	this->indices.clear();
	this->renderData.clear();
}

void Mesh::loadMesh(bool useDiffuseMap, bool drawIndexed, bool useNormalMap, bool useMaterialMap,
	bool useEmissiveMap, bool isStrippedNormal) 
{
	this->useDiffuseMap = useDiffuseMap;
	this->useNormalMap = useNormalMap;
	this->useMaterialMap = useMaterialMap;
	this->useEmissiveMap = useEmissiveMap;
	this->strippedNormalMap = isStrippedNormal;
	this->drawIndexed = drawIndexed;

	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texel));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

	if (this->drawIndexed == true) {
		glGenBuffers(1, &this->IBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(uint), &this->indices[0], GL_STATIC_DRAW);
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

void Mesh::renderMesh(Shader& shader, GLenum renderMode) {
	shader.setMat4("model", this->model);
	shader.setVec3("color", this->color);

	shader.setVec3("material.albedo", this->color);
	shader.setFloat("material.metallic", this->metallic);
	shader.setFloat("material.roughness", this->roughness);
	shader.setFloat("material.ao", this->ao);

	shader.setUint("useDiffuseMap", this->useDiffuseMap);
	shader.setUint("useNormalMap", this->useNormalMap);
	shader.setUint("strippedNormalMap", this->strippedNormalMap);
	shader.setUint("useMaterialMap", this->useMaterialMap);
	shader.setUint("useEmissiveMap", this->useEmissiveMap);

	this->drawMesh(renderMode);
}

void Mesh::renderMeshWithOutline(Shader& shader, Shader& outlineShader, GLenum renderMode)
{
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);

	this->renderMesh(shader, renderMode);

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

	shader.useShader();
}

void Mesh::clearMesh() {
	this->vertices.clear();
	this->indices.clear();

	delete this->diffuseMap;
	delete this->normalMap;
	delete this->depthMap;
	delete this->roughnessMap;
	delete this->metallicMap;

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