#include "ParticleRenderer.h"

ParticleRenderer::ParticleRenderer(ParticleTexture texture)
	: VAO{ 0 }, VBO{ 0 }, IBO{ 0 }, iVBO{ 0 }, texOffset1{ {} }, texOffset2{ {} }, blend{ 0.f }, pointer{ 0 } {
	this->texture = texture;
	this->texture.loadTexture();

	for (int i = 0; i < this->maxInstances * this->instanceDataLength; i++)
		this->dataBuffer[i] = NULL;

	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->vertices), this->vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	this->createEmptyVBO(this->instanceDataLength * this->maxInstances);

	this->addInstancedAttribute(1, 4, this->instanceDataLength, 0);
	this->addInstancedAttribute(2, 4, this->instanceDataLength, 4);
	this->addInstancedAttribute(3, 4, this->instanceDataLength, 8);
	this->addInstancedAttribute(4, 4, this->instanceDataLength, 12);
  
	this->addInstancedAttribute(5, 4, this->instanceDataLength, 16);
	this->addInstancedAttribute(6, 1, this->instanceDataLength, 20);
}

void ParticleRenderer::createEmptyVBO(int floatCount) {
	glBindVertexArray(this->VAO);
	glGenBuffers(1, &this->iVBO);

	glBindBuffer(GL_ARRAY_BUFFER, this->iVBO);
	glBufferData(GL_ARRAY_BUFFER, floatCount * sizeof(float), NULL, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void ParticleRenderer::addInstancedAttribute(int attribute, int dataSize, int instancedDataLength, int offset) {
	glBindBuffer(GL_ARRAY_BUFFER, this->iVBO);
	glBindVertexArray(this->VAO);

	glVertexAttribPointer(attribute, dataSize, GL_FLOAT, GL_FALSE,
		instancedDataLength * sizeof(float), (void*)(offset * sizeof(float)));
	glVertexAttribDivisor(attribute, 1);
	glEnableVertexAttribArray(attribute);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void ParticleRenderer::updateVBO() {
	glBindBuffer(GL_ARRAY_BUFFER, this->iVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(this->dataBuffer) * this->pointer, NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->dataBuffer) * this->pointer, this->dataBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	for (int i = 0; i < this->pointer; i++)
		this->dataBuffer[i] = NULL;

	this->pointer = 0;
}

void ParticleRenderer::updateTextureCoordInfo(const CParticle* const particle) {
	float lifeFactor = particle->getElapsedTime() / particle->getLifeTime();
	int stageCount = std::pow(this->texture.getNumberOfRows(), 2);
	float atlasProgression = lifeFactor * stageCount;

	int index1 = (int)std::floor(atlasProgression);
	int index2 = index1 < stageCount - 1 ? index1 + 1 : index1;

	this->blend = atlasProgression - std::floor(atlasProgression);

	this->setTextureOffset(this->texOffset1, index1);
	this->setTextureOffset(this->texOffset2, index2);

	this->dataBuffer[this->pointer++] = this->texOffset1.x;
	this->dataBuffer[this->pointer++] = this->texOffset1.y;
	this->dataBuffer[this->pointer++] = this->texOffset2.x;
	this->dataBuffer[this->pointer++] = this->texOffset2.y;

	this->dataBuffer[this->pointer++] = this->blend;
}

void ParticleRenderer::setTextureOffset(glm::vec2& offset, int index) {
	int col = index % (int)this->texture.getNumberOfRows();
	int row = index / this->texture.getNumberOfRows();

	offset.x = (float)col / this->texture.getNumberOfRows();
	offset.y = (float)row / this->texture.getNumberOfRows();
}

inline void ParticleRenderer::updateModelViewMatrix(mat4& modelView, mat4& view, vec3 position, 
	float rotation, float scale) {

	modelView = translate(modelView, position);

	// Billboarding
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++)
			modelView[i][j] = view[j][i];
	}

	modelView = rotate(modelView, rotation, vec3(0.f, 0.f, 1.f));
	modelView = glm::scale(modelView, vec3(scale, scale, scale));

	modelView = view * modelView;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			this->dataBuffer[this->pointer++] = modelView[i][j];
}

void ParticleRenderer::render(const std::vector<CParticle*>& particles, const Window* const currWindow,
	const Camera& camera, glm::mat4& modelViewMatrix, const glm::mat4& projection) {

	if (particles.size() == 0)
		return;

	mat4 view = camera.generateViewMatrix();

	this->shader.useShader();

	this->shader.setVec3("vColor", glm::vec3(1.f, 0.f, 0.f));
	this->shader.setMat4("projectionMatrix", projection);
	this->shader.setFloat("numberOfRows", this->texture.getNumberOfRows());
	
	for (auto& particle : particles) {
		modelViewMatrix = mat4(1.f);
		
		this->updateModelViewMatrix(modelViewMatrix, view, particle->getPosition(),
			particle->getRotation(), particle->getScale());
		this->updateTextureCoordInfo(&*particle);

		glm::vec4 texOffset{ this->texOffset1.x, this->texOffset1.y, this->texOffset2.x, this->texOffset2.y };

		this->texture.useTexture();
	}

	this->updateVBO();

	glDepthMask(false);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	this->drawParticleSprites(particles.size());

	glDepthMask(true);
	glDisable(GL_BLEND);
}

ParticleRenderer::~ParticleRenderer() {
	if (this->VBO != 0)
		glDeleteBuffers(1, &this->VBO);

	if (this->IBO != 0)
		glDeleteBuffers(1, &this->IBO);

	if (this->VAO != 0)
		glDeleteVertexArrays(1, &this->VAO);

	delete[] this->dataBuffer;
}