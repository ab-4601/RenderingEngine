#version 450 core

out uvec3 fragColor;

uniform uint objectIndex;
uniform uint drawIndex;

void main() {
	fragColor = uvec3(objectIndex, drawIndex, gl_PrimitiveID);
}