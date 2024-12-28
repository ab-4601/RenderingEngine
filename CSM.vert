#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 texCoords;

uniform mat4 model;

out VERT_DATA {
	vec2 texel;
} data_out;

void main() {
	gl_Position = model * vec4(aPos, 1.f);
	data_out.texel = texCoords;
}