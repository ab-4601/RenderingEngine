#version 450 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;

layout (std430, binding = 0) readonly buffer cameraSpaceVariables {
	mat4 projection;
	mat4 view;
	vec3 cameraPosition;
};

void main() {
	gl_Position = projection * view * model * vec4(aPos, 1.f);
}