#version 450 core
#extension GL_ARB_shader_draw_parameters : enable

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexel;

out VERT_DATA {
	vec2 texel;
	flat uint drawID;
} data_out;

uniform mat4 model;

void main() {
	gl_Position = model * vec4(aPos, 1.f);

	data_out.texel = aTexel;
	data_out.drawID = gl_DrawIDARB;
}