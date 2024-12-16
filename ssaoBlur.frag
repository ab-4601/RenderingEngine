#version 450 core

out float fragColor;

in vec2 texel;

uniform sampler2D ssao;

void main() {
	vec2 texelSize = 1.f / vec2(textureSize(ssao, 0));
	float result = 0.f;

	for(int x = -2; x < 2; x++) {
		for(int y = -2; y < 2; y++) {
			vec2 offset = vec2(float(x), float(y)) * texelSize;
			result += texture(ssao, texel + offset).r;
		}
	}

	fragColor = result / 16.f;
}