#version 450 core
#extension GL_NV_gpu_shader5 : enable

in GEOM_DATA {
	vec2 texel;
	flat uint drawID;
} data_in;

struct RenderData3D {
    int meshIndex;
    uint64_t diffuseMap;
    uint64_t normalMap;
    uint64_t metallicMap;
    uint64_t emissiveMap;
};

layout (std430, binding = 2) readonly buffer renderItems {
    RenderData3D renderData[];
};

uniform bool useDiffuseMap;

void main() {
    if(useDiffuseMap && texture2D(sampler2D(renderData[data_in.drawID].diffuseMap), data_in.texel).a < 0.1f)
        discard;
}