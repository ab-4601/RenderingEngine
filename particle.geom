#version 450 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out GS_OUT {
	vec4 color;
	vec2 texCoord1;
	vec2 texCoord2;
	float blend;
} gs_out;

in VS_OUT {
	vec3 color;
	vec2 texCoord1;
	vec2 texCoord2;
	float blend;
} data_in[];

void main() {
	gl_Position = gl_in[0].gl_Position;
	gs_out.color = vec4(data_in[0].color, 1);
	gs_out.blend = data_in[0].blend;
	gs_out.texCoord1 = data_in[0].texCoord1;
	gs_out.texCoord2 = data_in[0].texCoord2;
	EmitVertex();

	gl_Position = gl_in[1].gl_Position;
	gs_out.color = vec4(data_in[1].color, 1);
	gs_out.blend = data_in[1].blend;
	gs_out.texCoord1 = data_in[1].texCoord1;
	gs_out.texCoord2 = data_in[1].texCoord2;
	EmitVertex();

	gl_Position = gl_in[2].gl_Position;
	gs_out.color = vec4(data_in[2].color, 1);
	gs_out.blend = data_in[2].blend;
	gs_out.texCoord1 = data_in[2].texCoord1;
	gs_out.texCoord2 = data_in[2].texCoord2;
	EmitVertex();

	EndPrimitive();
}