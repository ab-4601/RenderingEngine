#version 450 core

layout ( triangles ) in;
layout ( triangle_strip, max_vertices = 3 ) out;

in DATA {
	vec4 color;
    vec2 texel;
    vec3 normal;
	vec3 tangent;
    vec4 fragPos;
} data_in[];

out GEOM_DATA {
	vec4 color;
    vec2 texel;
    vec3 normal;
	vec3 tangent;
    vec4 fragPos;
} data_out;

layout (std140, binding = 0) uniform cameraSpaceVariables {
	mat4 projection;
	mat4 view;
	vec3 cameraPosition;
};

uniform mat4 model;

void main() {

	for(int i = 0; i < 3; i++) {
		data_out.color = data_in[i].color;
		data_out.texel = data_in[i].texel;
		data_out.fragPos = model * data_in[i].fragPos;
		data_out.normal = mat3(transpose(inverse(model))) * data_in[i].normal;
		data_out.tangent = mat3(transpose(inverse(model))) * data_in[i].tangent;
		gl_Position = projection * view * model * gl_in[i].gl_Position;
		EmitVertex();
	}

	EndPrimitive();
}