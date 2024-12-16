#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexel;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;

out vec3 fragPos;
out vec2 texel;
out vec3 normal;
out vec3 tangent;
out vec3 color;
out mat3 normalMat;

layout (std140, binding = 0) uniform cameraSpaceMatrix {
	mat4 projection;
	mat4 view;
};

uniform mat4 model;
uniform vec3 vColor;

void main() {
	vec4 viewPos = view * model * vec4(aPos, 1.f);
	fragPos = viewPos.xyz;
	texel = aTexel;

	normalMat = transpose(inverse(mat3(view * model)));
	normal = normalMat * aNormal;
	tangent = normalMat * aTangent;

	color = vColor;

	gl_Position = projection * viewPos;
}