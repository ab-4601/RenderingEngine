#pragma once

#include "Core.h"
#include "Shader.h"
#include "Particle.h"
#include "Window.h"
#include "Camera.h"
#include "ParticleTexture.h"

class ParticleRenderer {
private:
	GLfloat vertices[12]{
		-0.5f,  0.5f,  0.f,
		 0.5f,  0.5f,  0.f,
		-0.5f, -0.5f,  0.f,
		 0.5f, -0.5f,  0.f
	};

	unsigned int indices[6]{
		0, 1, 2,
		1, 2, 3
	};

	static const int maxInstances = 10000;
	static const int instanceDataLength = 21;

	GLfloat* dataBuffer = new GLfloat[maxInstances * instanceDataLength];
	int pointer;

	GLuint VAO, VBO, IBO;
	GLuint iVBO;

	Shader shader{ "particle.vert", "particle.frag", "particle.geom" };
	ParticleTexture texture;

	glm::vec2 texOffset1;
	glm::vec2 texOffset2;
	float blend;

	void updateTextureCoordInfo(const CParticle* const particle);
	void setTextureOffset(glm::vec2& offset, int index);
	void updateModelViewMatrix(mat4& model, mat4& view, vec3 position, float rotation, float scale);

	void createEmptyVBO(int floatCount);
	void addInstancedAttribute(int attribute, int dataSize, int instancedDataLength, int offset);
	void updateVBO();

public:
	ParticleRenderer(ParticleTexture texture = ParticleTexture("Textures/cosmic.png"));

	void render(const std::vector<CParticle*>& particles, const Window* const currWindow,
		const Camera& camera, glm::mat4& modelViewMatrix, const glm::mat4& projection);
		
	inline float getBlend() const { return this->blend; }

	inline void setTexture(ParticleTexture& particleTexture) {
		this->texture = particleTexture;
		this->texture.loadTexture();
	}

	void drawParticleSprites(size_t primitiveCount) const {
		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL, static_cast<GLsizei>(primitiveCount));

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	~ParticleRenderer();
};