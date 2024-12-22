#version 450 core

layout (location = 0) in vec3 aPos;

out vec4 color;

layout (std140, binding = 0) uniform cameraSpaceVariables {
	mat4 projection;
	mat4 view;
	vec3 cameraPosition;
};

uniform mat4 model;
uniform vec3 lightColor;

void main() {
	color = vec4(lightColor, 1.f);

	gl_Position = projection * view * model * vec4(aPos, 1.f);
}