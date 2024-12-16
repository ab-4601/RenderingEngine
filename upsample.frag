#version 450 core

layout (location = 0) out vec3 upsample;

uniform sampler2D srcTexture;
uniform float filterRadius;
uniform float aspect = 1920.f / 1080.f;

in vec2 texel;

void main() {
	float x = filterRadius;
	float y = filterRadius * aspect;

	vec3 a = texture(srcTexture, vec2(texel.x - x, texel.y + y)).rgb;
    vec3 b = texture(srcTexture, vec2(texel.x,     texel.y + y)).rgb;
    vec3 c = texture(srcTexture, vec2(texel.x + x, texel.y + y)).rgb;

    vec3 d = texture(srcTexture, vec2(texel.x - x, texel.y)).rgb;
    vec3 e = texture(srcTexture, vec2(texel.x,     texel.y)).rgb;
    vec3 f = texture(srcTexture, vec2(texel.x + x, texel.y)).rgb;

    vec3 g = texture(srcTexture, vec2(texel.x - x, texel.y - y)).rgb;
    vec3 h = texture(srcTexture, vec2(texel.x,     texel.y - y)).rgb;
    vec3 i = texture(srcTexture, vec2(texel.x + x, texel.y - y)).rgb;

    upsample = e * 4.f;
    upsample += (b + d + f + h) * 2.f;
    upsample += (a + c + g + i);
    upsample *= 1.f / 16.f;
}