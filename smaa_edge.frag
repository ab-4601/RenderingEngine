#version 450 core

out vec4 fragColor;

in vec2 texel;

uniform sampler2D screenTexture;
uniform vec2 screenResolution;

void main() {
	vec2 texelStep = 1.f / screenResolution;

	vec3 color = texture2D(screenTexture, texel).rgb;
	vec3 colorRight = texture2D(screenTexture, texel + vec2(texelStep.x, 0.f)).rgb;
	vec3 colorDown = texture2D(screenTexture, texel + vec2(0.f, texelStep.y)).rgb;

	vec3 luminance = vec3(0.299f, 0.587f, 0.114f);

	float luma = dot(color, luminance);
	float lumaRight = dot(colorRight, luminance);
	float lumaDown = dot(colorDown, luminance);

	float edgeHorizontal = abs(luma - lumaDown);
	float edgeVertical = abs(luma - lumaRight);

	fragColor = vec4(edgeVertical, edgeHorizontal, 0.f, 1.f);
}