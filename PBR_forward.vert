#version 450 core
#extension GL_NV_gpu_shader5 : enable
#extension GL_ARB_shader_draw_parameters : enable

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 texel;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;

out VERT_DATA {
    vec4 color;
    vec2 texel;
    vec3 normal;
    vec3 tangent;
    vec4 fragPos;
    flat uint drawID;
} data_out;

layout (std430, binding = 0) readonly buffer cameraSpaceVariables {
	mat4 projection;
	mat4 view;
	vec3 cameraPosition;
};

uniform mat4 model;
uniform vec3 color;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
        
    data_out.color = vec4(color, 1.f);
    data_out.texel = texel;
    data_out.fragPos = model * vec4(aPos, 1.0);
    data_out.drawID = gl_BaseInstanceARB;
    data_out.normal = mat3(transpose(inverse(model))) * normal;
	data_out.tangent = mat3(transpose(inverse(model))) * tangent;
}