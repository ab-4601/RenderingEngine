#version 450 core

out float fragColor;

in vec2 texel;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D noise;

uniform vec3 samples[64];

uniform int kernelSize;
uniform float radius;
uniform float bias;
uniform float occlusionPower;

uniform vec2 screenRes;

const vec2 noiseScale = screenRes / 4.f;

uniform mat4 projection;

void main() {
	vec3 fragPos = texture(gPosition, texel).xyz;
	vec3 normal = normalize(texture(gNormal, texel).rgb);
	vec3 random = normalize(texture(noise, texel * noiseScale).xyz);

	vec3 tangent = normalize(random - normal * dot(random, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);

	float occlusion = 0.f;

	for(int i = 0; i < kernelSize; i++) {
		vec3 samplePos = TBN * samples[i];
		samplePos = fragPos + samplePos * radius;

		vec4 offset = vec4(samplePos, 1.f);
		offset = projection * offset;
		offset.xyz /= offset.w;
		offset.xyz = offset.xyz * 0.5f + 0.5f;

		float sampleDepth = texture(gPosition, offset.xy).z;
		float rangeCheck = smoothstep(0.f, 1.f, radius / abs(fragPos.z - sampleDepth));
		
		occlusion += (sampleDepth >= samplePos.z + bias ? 1.f : 0.f) * rangeCheck;
	}

	occlusion = 1.f - (occlusion / kernelSize);

	fragColor = pow(occlusion, occlusionPower);
}