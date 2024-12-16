#version 450 core

layout (location = 0) in vec3 aPos;

layout (std140, binding = 0) uniform cameraSpaceMatrix {
	mat4 projection;
	mat4 view;
};

out vec3 worldPos;
out vec3 cameraPos;
out float gridSizeVar;

uniform vec3 cameraPosition;
uniform float gridSize = 5000.f;

void main() {
	vec4 vPos = vec4(aPos, 1.f);

	vPos.xyz *= gridSize;

	vPos.x += cameraPosition.x;
	vPos.z += cameraPosition.z;

	worldPos = vPos.xyz;
	cameraPos = cameraPosition;
	gridSizeVar = gridSize;
	
	gl_Position = projection * view * vPos;
}