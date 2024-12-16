#include "Water.h"

Water::Water()
	: VAO{ 0 }, VBO{ 0 }, IBO{ 0 } {

	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->vertices), this->vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glGenBuffers(1, &this->IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), this->indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

GLuint Water::createFrameBuffer() {
	GLuint frameBuffer{};

	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	return frameBuffer;
}

GLuint Water::createTextureAttachment(int width, int height) {
	GLuint texture{};

	glGenTextures(1, &texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);

	return texture;
}

void Water::render(const glm::mat4& projection, glm::mat4& model, const Camera& camera) {
	glUseProgram(shader.getProgramID());

	model = glm::mat4(1.f);
	model = glm::translate(model, glm::vec3(0.f, 5.f, 0.f));

	glUniformMatrix4fv(shader.getUniformProjection(), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(shader.getUniformView(), 1, GL_FALSE, glm::value_ptr(camera.generateViewMatrix()));
	glUniformMatrix4fv(shader.getUniformModel(), 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(this->VAO);

	glDrawElements(GL_TRIANGLES, sizeof(this->indices) / sizeof(unsigned int), GL_UNSIGNED_INT, this->indices);

	glBindVertexArray(0);
}