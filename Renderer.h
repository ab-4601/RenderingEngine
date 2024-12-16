#pragma once

#include "Core.h"
#include "Mesh.h"

GLuint indirectBuffer{ 0 };

struct DrawElementsIndirectCommand {
	GLuint count;
	GLuint instanceCount;
	GLuint firstIndex;
	GLuint baseVertex;
	GLuint baseInstance;
};

std::vector<DrawElementsIndirectCommand> drawCommands;

void createIndirectBuffer() {
	glGenBuffers(1, &::indirectBuffer);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, ::indirectBuffer);

	for (const auto& mesh : Mesh::meshList) {
		DrawElementsIndirectCommand command = {
			
		};
	}
}