#include "Mesh.h"

Mesh::Mesh() {
	meshList.push_back(this);
	objectID = meshCount++;

	vertices.clear();
	indices.clear();
	meshData.clear();
	drawCommands.clear();
	renderData.clear();
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

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texel));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

	if (drawIndexed == true) {
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), &indices[0], GL_STATIC_DRAW);
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
	glBindVertexArray(VAO);

	if (drawIndexed) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glDrawElements(renderMode, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	else {
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glDrawArrays(renderMode, 0, static_cast<GLsizei>(vertices.size()));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	glBindVertexArray(0);
}

void Mesh::renderMesh(Shader& shader, GLenum renderMode) {
	shader.setMat4("model", model);
	shader.setVec3("color", color);

	shader.setVec3("material.albedo", color);
	shader.setFloat("material.metallic", metallic);
	shader.setFloat("material.roughness", roughness);
	shader.setFloat("material.ao", ao);

	shader.setUint("useDiffuseMap", useDiffuseMap);
	shader.setUint("useNormalMap", useNormalMap);
	shader.setUint("strippedNormalMap", strippedNormalMap);
	shader.setUint("useMaterialMap", useMaterialMap);
	shader.setUint("useEmissiveMap", useEmissiveMap);

	drawMesh(renderMode);
}

void Mesh::renderMeshWithOutline(Shader& shader, Shader& outlineShader, GLenum renderMode)
{
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);

	renderMesh(shader, renderMode);

	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	//glDisable(GL_DEPTH_TEST);

	outlineShader.useShader();

	outlineModel = glm::scale(model, glm::vec3(1.05f, 1.05f, 1.05f));
	outlineShader.setMat4("model", outlineModel);

	drawMesh(GL_TRIANGLES);

	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glDisable(GL_STENCIL_TEST);
	//glEnable(GL_DEPTH_TEST);

	outlineShader.endShader();

	shader.useShader();
}

void Mesh::clearMesh() {
	vertices.clear();
	indices.clear();

	delete diffuseMap;
	delete normalMap;
	delete depthMap;
	delete roughnessMap;
	delete metallicMap;

	if (EBO != 0) {
		glDeleteBuffers(1, &EBO);
		EBO = 0;
	}

	if (VBO != 0) {
		glDeleteBuffers(1, &VBO);
		VBO = 0;
	}

	if (IBO != 0) {
		glDeleteBuffers(1, &IBO);
		IBO = 0;
	}

	if (VAO != 0) {
		glDeleteVertexArrays(1, &VAO);
		VAO = 0;
	}
}