#version 450 core

layout ( triangles ) in;
layout ( triangle_strip, max_vertices = 3 ) out;

in DATA {
	vec4 vColor;
    vec2 texel;
    vec3 normal;
	vec3 tangent;
    vec4 fragPos;
} data_in[];

out GEOM_DATA {
	vec4 vColor;
    vec2 texel;
    vec3 normal;
	vec3 tangent;
    vec4 fragPos;
	vec4 lightSpaceFragPos;
} data_out;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 lightSpaceTransform;

void main() {

	for(int i = 0; i < 3; i++) {
		data_out.vColor = data_in[i].vColor;
		data_out.texel = data_in[i].texel;
		data_out.fragPos = model * data_in[i].fragPos;
		data_out.normal = mat3(transpose(inverse(model))) * data_in[i].normal;
		data_out.tangent = mat3(transpose(inverse(model))) * data_in[i].tangent;
		data_out.lightSpaceFragPos = lightSpaceTransform * data_out.fragPos;
		gl_Position = projection * view * model * gl_in[i].gl_Position;
		EmitVertex();
	}

	EndPrimitive();
}