#version 450 core

out vec4 fragColor;

in vec2 texel;

uniform sampler2D screenTexture;
uniform vec2 screenResolution;

void main() {
	vec2 texelStep = 1.f / screenResolution;

	vec4 color = texture2D(screenTexture, texel);

	vec4 left = textureOffset(screenTexture, texel, ivec2(-1, 0));
	vec4 right = textureOffset(screenTexture, texel, ivec2(1, 0));
	vec4 up = textureOffset(screenTexture, texel, ivec2(0, 1));
	vec4 down = textureOffset(screenTexture, texel, ivec2(0, -1));
}