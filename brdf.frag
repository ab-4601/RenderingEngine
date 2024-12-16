#version 450 core

out vec2 fragColor;
in vec2 texel;

const float PI = 3.14159265359;

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

float geometrySchlick(float NdotV, float roughness) {
	float a = roughness;
	float k = (a * a) / 2.f;

	float numerator = NdotV;
	float denominator = NdotV * (1.f - k) + k;

	return numerator / denominator;
}

float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
	float NdotV = max(dot(N, V), 0.f);
	float NdotL = max(dot(N, L), 0.f);
	float ggx2 = geometrySchlick(NdotV, roughness);
	float ggx1 = geometrySchlick(NdotL, roughness);

	return ggx1 * ggx2;
}

vec2 integrateBRDF(float NdotV, float roughness) {
	vec3 V = vec3(0.f);
	V.x = sqrt(1.f - NdotV * NdotV);
	V.y = 0.f;
	V.z = NdotV;

	float A = 0.f, B = 0.f;

	vec3 N = vec3(0.f, 0.f, 1.f);

	const uint SAMPLE_COUNT = 1024u;

	for(uint i = 0u; i < SAMPLE_COUNT; i++) {
		vec2 xi = hammersley(i, SAMPLE_COUNT);
		vec3 H = ImportanceSampleGGX(xi, N, roughness);
		vec3 L = normalize(2.f * dot(V, H) * H - V);

		float NdotL = max(L.z, 0.f);
		float NdotH = max(H.z, 0.f);
		float VdotH = max(dot(V, H), 0.f);

		if(NdotL > 0.f) {
			float G = geometrySmith(N, V, L, roughness);
			float G_vis = (G * VdotH) / (NdotH * NdotV);
			float Fc = pow(1.f - VdotH, 5.f);

			A += (1.f - Fc) * G_vis;
			B += Fc * G_vis;
		}
	}

	A /= float(SAMPLE_COUNT);
	B /= float(SAMPLE_COUNT);

	return vec2(A, B);
}

void main() {
	fragColor = integrateBRDF(texel.x, texel.y);
}