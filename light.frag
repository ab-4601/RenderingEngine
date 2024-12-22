#version 450 core

in vec4 color;

out vec4 fragColor;

uniform float intensity;

void main() {
	fragColor = color * intensity;
}