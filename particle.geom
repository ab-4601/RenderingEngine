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
	for(int i = 0; i < 3; i++) {
		gl_Position = gl_in[i].gl_Position;
		gs_out.color = vec4(data_in[i].color, 1);
		gs_out.blend = data_in[i].blend;
		gs_out.texCoord1 = data_in[i].texCoord1;
		gs_out.texCoord2 = data_in[i].texCoord2;
		EmitVertex();
	}
	
	EndPrimitive();
}