#version 450 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;
layout (location = 3) out vec3 gMetallic;

in vec2 texel;
in vec3 fragPos;
in vec3 normal;
in vec3 tangent;
in vec3 color;
in mat3 normalMat;

uniform bool useDiffuseMap;
uniform bool useNormalMap;
uniform bool useMetalnessMap;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D metalnessMap;

void main() {
	gPosition = fragPos / 7000.f;

	if(useNormalMap) {
		vec3 vNormal = texture(normalMap, texel).rgb;
		vNormal = normalize(vNormal * 2.f - 1.f);
		//vNormal = normalize(1.f - vNormal * 2.f);
		
		vec3 T = normalize(tangent);
		vec3 N = normalize(normal);

		T = normalize(T - dot(T, N) * N);
		vec3 B = cross(N, T);

		vNormal = normalize(mat3(T, B, N) * vNormal);
		gNormal = vNormal;
	}
	else
		gNormal = normalize(normal);

	if(useDiffuseMap)
		gAlbedo = texture(diffuseMap, texel).rgb;
	else
		gAlbedo = color;

	if(useMetalnessMap)
		gMetallic = texture(metalnessMap, texel).rgb;
	else
		gMetallic = vec3(0.95f);
}