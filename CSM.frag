#version 450 core

in GEOM_DATA {
	vec2 texel;
} data_in;

uniform sampler2D diffuseMap;

void main() {
	if(texture2D(diffuseMap, data_in.texel).a < 0.1f)
		discard;
}