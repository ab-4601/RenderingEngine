#version 450 core

in vec2 textureCoord;

out vec4 fragColor;

uniform sampler2D textureSampler;

void main() {
	vec4 color = texture2D(textureSampler, textureCoord);

	if(color.a < 0.1)
		discard;

	fragColor = vec4(0.f, 1.f, 1.f, 1.f);
}