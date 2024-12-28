#version 450 core

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;
layout (location = 3) out vec3 gMetallic;

const float wireframeWidth = 0.7f;
const vec3 wireframeColor = vec3(0.f, 1.f, 1.f);

in GEOM_DATA {
	vec4 fragPos;
	vec2 texel;
	vec3 normal;
	vec3 tangent;
	vec3 color;
	noperspective vec3 edgeDistance;
} data_in;

uniform bool useDiffuseMap;
uniform bool useNormalMap;
uniform bool useMaterialMap;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;

uniform bool strippedNormalMap;
uniform bool drawWireframe;

void main() {
	if(useDiffuseMap) {
		vec4 texColor = texture2D(diffuseMap, data_in.texel);

		if(texColor.a < 0.1f)
			discard;
		else
			gAlbedo = texColor.rgb;
	}
	else
		gAlbedo = data_in.color;

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
		vec3 vNormal = texture2D(normalMap, data_in.texel).rgb;
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
		gMetallic = texture2D(metallicMap, data_in.texel).rgb;
	else
		gMetallic = vec3(0.95f);

	gPosition = data_in.fragPos;
}