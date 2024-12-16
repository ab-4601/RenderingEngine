#version 450 core

layout (location = 0) in vec3 aPos;

uniform mat4 transformation;

out vec2 textureCoord;

void main() {
	gl_Position = transformation * vec4(aPos, 1.0);
	textureCoord = vec2((aPos.x + 1) / 2.f, 1 - (aPos.y + 1) / 2.f);
}