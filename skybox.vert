#version 450 core

layout (location = 0) in vec3 aPos;

out vec3 texel;

layout (std430, binding = 0) readonly buffer cameraSpaceVariables {
	mat4 projection;
	mat4 view;
	vec3 cameraPosition;
};

void main() {
	texel = aPos;

	mat4 modifiedView = mat4(mat3(view));

	vec4 pos = projection * modifiedView * vec4(aPos, 1.f);
	gl_Position = pos.xyww;
}