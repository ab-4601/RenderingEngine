#version 450 core

layout (location = 0) in vec3 aPos;

out vec4 color;

layout (std140, binding = 0) uniform cameraSpaceVariables {
	mat4 projection;
	mat4 view;
	vec3 cameraPosition;
};

uniform vec3 vColor;
uniform mat4 model;

void main() {
	gl_Position = projection * view * model * vec4(aPos, 1.f);

	color = vec4(vColor, 1.f);
}