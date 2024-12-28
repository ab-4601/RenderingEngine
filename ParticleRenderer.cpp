#include "ParticleRenderer.h"

ParticleRenderer::ParticleRenderer(ParticleTexture texture)
	: VAO{ 0 }, VBO{ 0 }, IBO{ 0 }, iVBO{ 0 }, texOffset1{ {} }, texOffset2{ {} }, blend{ 0.f }, pointer{ 0 } {
	this->texture = texture;
	this->texture.loadTexture();

	for (int i = 0; i < maxInstances * instanceDataLength; i++)
		dataBuffer[i] = NULL;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	createEmptyVBO(instanceDataLength * maxInstances);

	addInstancedAttribute(1, 4, instanceDataLength, 0);
	addInstancedAttribute(2, 4, instanceDataLength, 4);
	addInstancedAttribute(3, 4, instanceDataLength, 8);
	addInstancedAttribute(4, 4, instanceDataLength, 12);
  
	addInstancedAttribute(5, 4, instanceDataLength, 16);
	addInstancedAttribute(6, 1, instanceDataLength, 20);
}

void ParticleRenderer::createEmptyVBO(int floatCount) {
	glBindVertexArray(VAO);
	glGenBuffers(1, &iVBO);

	glBindBuffer(GL_ARRAY_BUFFER, iVBO);
	glBufferData(GL_ARRAY_BUFFER, floatCount * sizeof(float), NULL, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void ParticleRenderer::addInstancedAttribute(int attribute, int dataSize, int instancedDataLength, int offset) {
	glBindBuffer(GL_ARRAY_BUFFER, iVBO);
	glBindVertexArray(VAO);

	glVertexAttribPointer(attribute, dataSize, GL_FLOAT, GL_FALSE,
		instancedDataLength * sizeof(float), (void*)(offset * sizeof(float)));
	glVertexAttribDivisor(attribute, 1);
	glEnableVertexAttribArray(attribute);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void ParticleRenderer::updateVBO() {
	glBindBuffer(GL_ARRAY_BUFFER, iVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(dataBuffer) * pointer, NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(dataBuffer) * pointer, dataBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	for (int i = 0; i < pointer; i++)
		dataBuffer[i] = NULL;

	pointer = 0;
}

void ParticleRenderer::updateTextureCoordInfo(const CParticle* const particle) {
	float lifeFactor = particle->getElapsedTime() / particle->getLifeTime();
	int stageCount = static_cast<int>(std::pow(texture.getNumberOfRows(), 2));
	float atlasProgression = lifeFactor * stageCount;

	int index1 = (int)std::floor(atlasProgression);
	int index2 = index1 < stageCount - 1 ? index1 + 1 : index1;

	blend = atlasProgression - std::floor(atlasProgression);

	setTextureOffset(texOffset1, index1);
	setTextureOffset(texOffset2, index2);

	dataBuffer[pointer++] = texOffset1.x;
	dataBuffer[pointer++] = texOffset1.y;
	dataBuffer[pointer++] = texOffset2.x;
	dataBuffer[pointer++] = texOffset2.y;

	dataBuffer[pointer++] = blend;
}

void ParticleRenderer::setTextureOffset(glm::vec2& offset, int index) {
	int col = index % (int)texture.getNumberOfRows();
	int row = index / (int)texture.getNumberOfRows();

	offset.x = (float)col / texture.getNumberOfRows();
	offset.y = (float)row / texture.getNumberOfRows();
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
			dataBuffer[pointer++] = modelView[i][j];
}

void ParticleRenderer::render(const std::vector<CParticle*>& particles, const Window* const currWindow,
	const Camera& camera, glm::mat4& modelViewMatrix, const glm::mat4& projection) {

	if (particles.size() == 0)
		return;

	mat4 view = camera.generateViewMatrix();

	shader.useShader();

	shader.setVec3("vColor", glm::vec3(1.f, 0.f, 0.f));
	shader.setMat4("projectionMatrix", projection);
	shader.setFloat("numberOfRows", texture.getNumberOfRows());
	
	for (auto& particle : particles) {
		modelViewMatrix = mat4(1.f);
		
		updateModelViewMatrix(modelViewMatrix, view, particle->getPosition(),
			particle->getRotation(), particle->getScale());
		updateTextureCoordInfo(&*particle);

		glm::vec4 texOffset{ texOffset1.x, texOffset1.y, texOffset2.x, texOffset2.y };

		texture.useTexture();
	}

	updateVBO();

	glDepthMask(false);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	drawParticleSprites(particles.size());

	glDepthMask(true);
	glDisable(GL_BLEND);
}

ParticleRenderer::~ParticleRenderer() {
	if (VBO != 0)
		glDeleteBuffers(1, &VBO);

	if (IBO != 0)
		glDeleteBuffers(1, &IBO);

	if (VAO != 0)
		glDeleteVertexArrays(1, &VAO);

	delete[] dataBuffer;
}