#version 450 core

out vec4 fragColor;

in vec2 texel;

uniform sampler2D image;

uniform bool horizontal;
uniform float weight[5] = float[] (0.227027f, 0.1945946f, 0.1216216f, 0.054054f, 0.016216f);

void main() {
	vec2 texOffset = 1.f / textureSize(image, 0);
	vec3 result = texture(image, texel).rgb * weight[0];

	if(horizontal) {
		for(int i = 1; i < 5; i++) {
			result += texture(image, texel + vec2(texOffset.x * i, 0.f)).rgb * weight[i];
			result += texture(image, texel - vec2(texOffset.x * i, 0.f)).rgb * weight[i];
		}
	}
	else {
		for(int i = 1; i < 5; i++) {
			result += texture(image, texel + vec2(0.f, texOffset.y * i)).rgb * weight[i];
			result += texture(image, texel - vec2(0.f, texOffset.y * i)).rgb * weight[i];
		}
	}

	fragColor = vec4(result, 1.f);
}