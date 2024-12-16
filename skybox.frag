#version 450 core

out vec4 fragColor;

in vec3 texel;

uniform samplerCube environmentMap;

void main() {
	vec3 finalColor = texture(environmentMap, texel).rgb;
	fragColor = vec4(finalColor, 1.f);
}