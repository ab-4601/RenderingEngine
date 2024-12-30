#version 450 core

layout (triangles, invocations = 5) in;
layout (triangle_strip, max_vertices = 3) out;

layout (std430, binding = 1) buffer LightSpaceMatrices {
	mat4 lightSpaceMatrices[16];
};

in VERT_DATA {
	vec2 texel;
	flat uint drawID;
} data_in[];

out GEOM_DATA {
	vec2 texel;
	flat uint drawID;
} data_out;

void main() {
	for(int i = 0; i < 3; i++) {
		gl_Position = lightSpaceMatrices[gl_InvocationID] * gl_in[i].gl_Position;
		gl_Layer = gl_InvocationID;
		data_out.texel = data_in[i].texel;
		data_out.drawID = data_in[i].drawID;

		EmitVertex();
	}

	EndPrimitive();
}