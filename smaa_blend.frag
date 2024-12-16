#version 450 core

out vec4 fragColor;

in vec2 texel;

uniform sampler2D edgeTexture;
uniform vec2 screenResolution;

void main() {
	vec2 texelStep = 1.f / screenResolution;
	vec4 edge = texture2D(edgeTexture, texel);
	vec2 weights = vec2(0.f);

	if(edge.r > 0.f) {
		vec2 offset = vec2(texelStep.x, 0.f);
		for(int i = 0; i < 16; i++) {
			vec2 samplePos = texel + offset * float(i);
			if(texture2D(edgeTexture, samplePos).r <= 0.f) break;
			weights.x += 1.f;
		}
	}

	if(edge.g > 0.f) {
		vec2 offset = vec2(0.f, texelStep.y);
		for(int i = 0; i < 16; i++) {
			vec2 samplePos = texel + offset * float(i);
			if(texture2D(edgeTexture, samplePos).g <= 0.f) break;
			weights.x += 1.f;
		}
	}

	weights /= 16.f;

	fragColor = vec4(weights, 0.f, 1.f);
}