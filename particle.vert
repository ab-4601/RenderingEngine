#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in mat4 modelViewMatrix;
layout (location = 5) in vec4 texOffset;
layout (location = 6) in float blendFactor;

uniform vec3 vColor;
uniform mat4 projectionMatrix;
uniform float numberOfRows;

out VS_OUT {
	vec3 color;
	vec2 texCoord1;
	vec2 texCoord2;
	float blend;
} data_out;

void main() {
	gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0);

	vec2 textureCoords = position.xy + vec2(0.5f, 0.5f);
	textureCoords.y = 1.f - textureCoords.y;

	textureCoords /= numberOfRows;

	data_out.texCoord1 = textureCoords + texOffset.xy;
	data_out.texCoord2 = textureCoords + texOffset.zw;
	data_out.blend = blendFactor;
	data_out.color = vColor;
}