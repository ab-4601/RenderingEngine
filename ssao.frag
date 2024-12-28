#version 450 core

out float fragColor;

in vec2 texel;

layout (binding = 0) uniform sampler2D gPosition;
layout (binding = 1) uniform sampler2D gNormal;
layout (binding = 2) uniform sampler2D noise;

uniform vec3 samples[64];
uniform vec2 screenRes;

uniform int kernelSize;
uniform float radius;
uniform float bias;
uniform float occlusionPower;

const vec2 noiseScale = screenRes / vec2(textureSize(noise, 0));

layout (std140, binding = 0) uniform cameraSpaceVariables {
	mat4 projection;
	mat4 view;
	vec3 cameraPosition;
};

void main() {
	vec4 fragPos = view * texture2D(gPosition, texel);
	vec3 normal = normalize(mat3(view) * texture2D(gNormal, texel).rgb);
	vec3 random = texture2D(noise, texel * noiseScale).rgb;

	vec3 tangent = normalize(random - normal * dot(random, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);

	float occlusion = 0.f;

	for(int i = 0; i < kernelSize; i++) {
		vec3 samplePos = TBN * samples[i];
		samplePos = fragPos.xyz + samplePos * radius;

		vec4 offset = vec4(samplePos, 1.f);
		offset = projection * offset;
		offset.xyz /= offset.w;
		offset.xyz = offset.xyz * 0.5f + 0.5f;

		float sampleDepth = vec3(view * texture2D(gPosition, offset.xy)).z;
		float rangeCheck = smoothstep(0.f, 1.f, radius / abs(fragPos.z - sampleDepth));
		
		occlusion += (sampleDepth >= samplePos.z + bias ? 1.f : 0.f) * rangeCheck;
	}

	occlusion = 1.f - (occlusion / kernelSize);

	occlusion = pow(occlusion, occlusionPower);

	fragColor = occlusion;
}