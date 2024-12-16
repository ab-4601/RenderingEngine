#version 450 core

const float PI = 3.14159265359;

out vec4 fragColor;
in vec3 texel;

uniform samplerCube environmentMap;
uniform float cubemapResolution;
uniform float roughness;

float distributionGGX(vec3 N, vec3 H, float roughness) {
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.f);
	float NdotH2 = NdotH * NdotH;

	float numerator = a2;
	float denominator = (NdotH * (a2 - 1.f) + 1.f);
	denominator = PI * denominator * denominator;

	return numerator / denominator;
}

float RadicalInverse(uint bits) {
	bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);

    return float(bits) * 2.3283064365386963e-10;
}

vec2 hammersley(uint i, uint N) {
	return vec2(float(i) / float(N), RadicalInverse(i));
}

vec3 ImportanceSampleGGX(vec2 xi, vec3 N, float roughness) {
	float a = roughness * roughness;

	float phi = 2.f * PI * xi.x;
	float cosTheta = sqrt((1.f - xi.y) / (1.f + (a * a - 1.f) * xi.y));
	float sinTheta = sqrt(1.f - cosTheta * cosTheta);

	vec3 H = vec3(0.f);
	H.x = cos(phi) * sinTheta;
	H.y = sin(phi) * sinTheta;
	H.z = cosTheta;

	vec3 up = abs(N.z) < 0.999f ? vec3(0.f, 0.f, 1.f) : vec3(1.f, 0.f, 0.f);
	vec3 tangent = normalize(cross(up, N));
	vec3 bitangent = cross(N, tangent);

	vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;

	return normalize(sampleVec);
}

void main() {
	vec3 N = normalize(texel);
	vec3 R = N;
	vec3 V = R;

	const uint SAMPLE_COUNT = 1024u;
	float totalWeight = 0.f;
	vec3 prefilteredColor = vec3(0.f);

	for(uint i = 0u; i < SAMPLE_COUNT; i++) {
		vec2 xi = hammersley(i, SAMPLE_COUNT);
		vec3 H = ImportanceSampleGGX(xi, N, roughness);
		vec3 L = normalize(2.f * dot(V, H) * H - V);

		float NdotL = max(dot(N, L), 0.f);

		if(NdotL > 0.f) {
			float D = distributionGGX(N, H, roughness);
			float NdotH = max(dot(N, H), 0.f);
			float HdotV = max(dot(H, V), 0.f);
			float pdf = D * NdotH / (4.f * HdotV) + 0.0001f;

			float saTexel = 4.f * PI / (6.f * cubemapResolution * cubemapResolution);
			float saSample = 1.f / (float(SAMPLE_COUNT) * pdf * 0.0001f);

			float mipLevel = roughness == 0.f ? 0.f : 0.5f * log2(saSample / saTexel);

			prefilteredColor += textureLod(environmentMap, L, mipLevel).rgb * NdotL;
			totalWeight += NdotL;
		}
	}

	prefilteredColor /= totalWeight;

	fragColor = vec4(prefilteredColor, 1.f);
}