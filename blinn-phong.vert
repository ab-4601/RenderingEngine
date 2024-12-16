#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 texel;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;

out DATA {
    vec4 vColor;
    vec2 texel;
    vec3 normal;
    vec3 tangent;
    vec4 fragPos;
} data_out;

uniform vec3 color;

void main() {
    gl_Position = vec4(aPos, 1.0);
    
    data_out.vColor = vec4(color, 1.f);
    data_out.texel = texel;
    data_out.fragPos = vec4(aPos, 1.0);
    data_out.normal = normal;
    data_out.tangent = tangent;
}