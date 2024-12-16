#version 450 core

out vec4 fragColor;

in vec2 texel;

layout (binding = 0) uniform sampler2D hdrBuffer;
layout (binding = 1) uniform sampler2D bloomBuffer;
layout (binding = 2) uniform sampler2D blendWeightTexture;
uniform vec2 screenResolution;

uniform float exposure;
uniform bool enableBloom;

const float gamma = 2.2f;
const float FXAA_REDUCE_MUL = 1.f / 8.f;
const float FXAA_REDUCE_MIN = 1.f / 128.f;
const vec2 FXAA_SPOIL = vec2(1.f);

vec3 FXAA() {
	vec3 color = texture2D(hdrBuffer, texel).rgb;
	vec3 luma = vec3(0.299f, 0.587f, 0.114f);

	vec2 texelOffset = 1.f / vec2(textureSize(hdrBuffer, 0));

	vec2 posM1 = texel - texelOffset;
	vec2 posP1 = texel + texelOffset;
	vec2 posM0 = texel - vec2(texelOffset.x, 0.f);
	vec2 posP0 = texel + vec2(texelOffset.x, 0.f);
	vec2 pos0M = texel - vec2(0.f, texelOffset.y);
	vec2 pos0P = texel + vec2(0.f, texelOffset.y);

	float lumaM1 = dot(texture2D(hdrBuffer, posM1).rgb, luma);
	float lumaP1 = dot(texture2D(hdrBuffer, posP1).rgb, luma);
	float lumaM0 = dot(texture2D(hdrBuffer, posM0).rgb, luma);
	float lumaP0 = dot(texture2D(hdrBuffer, posP0).rgb, luma);
	float luma0M = dot(texture2D(hdrBuffer, pos0M).rgb, luma);
	float luma0P = dot(texture2D(hdrBuffer, pos0P).rgb, luma);

	float lumaCenter = dot(color, luma);

	float edgeHorizontal = abs(lumaM1 + lumaP1 - 2.f * lumaCenter);
	float edgeVertical = abs(luma0M + luma0P - 2.f * lumaCenter);
	float edgeStrength = max(edgeHorizontal, edgeVertical);

	float blendFactor = clamp(edgeStrength, 0.f, 1.f);
	color = mix(color, (texture2D(hdrBuffer, posM1).rgb + texture2D(hdrBuffer, posP1).rgb) * 0.5f, blendFactor);

	return color;
}

vec3 SMAA() {
	vec3 color = texture2D(hdrBuffer, texel).rgb;
	vec4 weights = texture2D(blendWeightTexture, texel);

	vec2 texelStep = 1.f / screenResolution;
	vec3 blendedColor = color;

	//Horizontal blend
	blendedColor += weights.r * texture2D(hdrBuffer, texel + vec2(texelStep.x, 0.f)).rgb;
	blendedColor += weights.g * texture2D(hdrBuffer, texel - vec2(texelStep.x, 0.f)).rgb;

	//Vertical blend
	blendedColor += weights.b * texture2D(hdrBuffer, texel + vec2(0.f, texelStep.y)).rgb;
	blendedColor += weights.a * texture2D(hdrBuffer, texel - vec2(0.f, texelStep.y)).rgb;

	return blendedColor;
}

mat3 aces_input_matrix = mat3(
	vec3(0.59719f, 0.35458f, 0.04823f),
    vec3(0.07600f, 0.90834f, 0.01566f),
    vec3(0.02840f, 0.13383f, 0.83777f)
);

mat3 aces_output_matrix = mat3(
	vec3( 1.60475f, -0.53108f, -0.07367f),
    vec3(-0.10208f,  1.10813f, -0.00605f),
    vec3(-0.00327f, -0.07276f,  1.07602f)
);

vec3 mul(const mat3 m, vec3 v) {
	float x = m[0][0] * v[0] + m[0][1] * v[1] + m[0][2] * v[2];
    float y = m[1][0] * v[0] + m[1][1] * v[1] + m[1][2] * v[2];
    float z = m[2][0] * v[0] + m[2][1] * v[1] + m[2][2] * v[2];

    return vec3(x, y, z);
}

vec3 rtt_and_odt_fit(vec3 v) {
	vec3 a = v * (v + 0.0245786f) - 0.000090537f;
    vec3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
    return a / b;
}

vec3 aces(vec3 color) {
	color = mul(aces_input_matrix, color);
    color = rtt_and_odt_fit(color);
    return mul(aces_output_matrix, color);
}

void main() {
	vec3 hdrColor = texture2D(hdrBuffer, texel).rgb;
	//vec3 hdrColor = SMAA();
	vec3 bloomColor = texture2D(bloomBuffer, texel).rgb;

	if(enableBloom)
		hdrColor = mix(hdrColor, bloomColor, 0.04f);

	vec3 mapped = aces(hdrColor);

	mapped = pow(mapped, vec3(1.f / gamma)) * exposure;

	fragColor = vec4(mapped, 1.f);
}