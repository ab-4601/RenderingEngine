#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexel;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;

out VERT_DATA {
	vec4 fragPos;
	vec2 texel;
	vec3 normal;
	vec3 tangent;
	vec3 color;
} data_out;

layout (std140, binding = 0) uniform cameraSpaceVariables {
	mat4 projection;
	mat4 view;
	vec3 cameraPosition;
};

uniform mat4 model;
uniform vec3 color;

void main() {
	vec4 worldPos = model * vec4(aPos, 1.f);
	data_out.fragPos = worldPos;
	data_out.texel = aTexel;

	mat3 normalMat = transpose(inverse(mat3(model)));
	data_out.normal = normalMat * aNormal;
	data_out.tangent = normalMat * aTangent;

	data_out.color = color;

	gl_Position = projection * view * worldPos;
}