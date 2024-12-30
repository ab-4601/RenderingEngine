#version 450 core
#extension GL_NV_gpu_shader5 : enable

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;
layout (location = 3) out vec3 gMetallic;

const float wireframeWidth = 0.7f;
const vec3 wireframeColor = vec3(0.f, 1.f, 1.f);

in GEOM_DATA {
	vec4 color;
    vec2 texel;
    vec3 normal;
	vec3 tangent;
    vec4 fragPos;
	flat uint drawID;
	noperspective vec3 edgeDistance;
} data_in;

uniform bool useDiffuseMap;
uniform bool useNormalMap;
uniform bool useMaterialMap;

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

uniform bool strippedNormalMap;
uniform bool drawWireframe;

void main() {
	if(useDiffuseMap) {
		vec4 texColor = texture2D(sampler2D(renderData[data_in.drawID].diffuseMap), data_in.texel);

		if(texColor.a < 0.1f)
			discard;
		else
			gAlbedo = texColor.rgb;
	}
	else
		gAlbedo = data_in.color.rgb;

	float d = 0.f, mixVal = 0.f;

	if(drawWireframe) {
		d = min(data_in.edgeDistance.x, data_in.edgeDistance.y);
		d = min(d, data_in.edgeDistance.z);

		if(d < wireframeWidth - 1.f)
			mixVal = 1.f;
		else if(d > wireframeWidth + 1.f)
			mixVal = 0.f;
		else {
			float x = d - (wireframeWidth - 1.f);
			mixVal = exp2(-2.f * x * x);
		}

		gAlbedo = mix(gAlbedo, wireframeColor, mixVal);
	}

	if(useNormalMap) {
		vec3 vNormal = texture2D(sampler2D(renderData[data_in.drawID].normalMap), data_in.texel).rgb;
		vNormal = normalize(vNormal * 2.f - 1.f);

		if(strippedNormalMap) vNormal *= -1.f;
		
		vec3 T = normalize(data_in.tangent);
		vec3 N = normalize(data_in.normal);

		T = normalize(T - dot(T, N) * N);
		vec3 B = cross(N, T);

		vNormal = normalize(mat3(T, B, N) * vNormal);

		gNormal = vNormal;
	}
	else
		gNormal = normalize(data_in.normal);

	if(useMaterialMap)
		gMetallic = texture2D(sampler2D(renderData[data_in.drawID].metallicMap), data_in.texel).rgb;
	else
		gMetallic = vec3(0.95f);

	gPosition = data_in.fragPos;
}