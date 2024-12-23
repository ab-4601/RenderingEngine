#version 450 core

#define PI 3.1415926535897932384626433832795

const int MAX_POINT_LIGHTS = 3;
const int MAX_SPOT_LIGHTS = 3;
const int MAX_CASCADES = 16;
const float wireframeWidth = 0.5f;
const vec4 wireframeColor = vec4(0.f, 1.f, 1.f, 1.f);

out vec4 fragColor;

in GEOM_DATA {
	vec4 color;
    vec2 texel;
    vec3 normal;
	vec3 tangent;
    vec4 fragPos;
} data_in;

noperspective in vec3 edgeDistance;

struct Light {
	vec3 color;
};

struct DirectionalLight  {
	Light base;
	vec3 direction;
};

struct PointLight {
	Light base;
	vec3 position;
	float constant;
	float linear;
	float exponent;
};

struct SpotLight {
	PointLight base;
	vec3 direction;
	float edge;
};

struct Material {
	vec3 albedo;
	float metallic;
	float roughness;
	float ao;
};

uniform int pointLightCount;
uniform int spotLightCount;

uniform DirectionalLight directionalLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];
uniform Material material;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D emissiveMap;
layout (binding = 1) uniform samplerCube irradianceMap;
layout (binding = 2) uniform samplerCube prefilterMap;
layout (binding = 3) uniform sampler2D brdfLUT;
layout (binding = 4) uniform samplerCube pointShadowMap;
layout (binding = 5) uniform sampler2DArray cascadedShadowMap;
layout (binding = 6) uniform sampler3D randomOffsets;

uniform bool useDiffuseMap;
uniform bool useNormalMap;
uniform bool strippedNormalMap;
uniform bool useMaterialMap;
uniform bool calcShadows;
uniform bool enableSSAO;
uniform bool drawWireframe;

uniform float nearPlane;
uniform float farPlane;
uniform int cascadeCount;
uniform float cascadePlanes[MAX_CASCADES];
uniform float radius;
uniform vec3 offsetTexSize;

layout (std140, binding = 0) uniform cameraSpaceVariables {
	mat4 projection;
	mat4 view;
	vec3 cameraPosition;
};

layout (std140, binding = 1) buffer LightSpaceMatrices {
	mat4 lightSpaceMatrices[16];
};

vec3 N = normalize(data_in.normal);
vec3 L = vec3(0.f);
vec3 H = vec3(0.f);
vec3 V = normalize(cameraPosition - vec3(data_in.fragPos));
vec3 F0 = vec3(0.04f);

vec3 albedo = material.albedo;
float metallic = material.metallic;
float roughness = material.roughness;
float ao = material.ao;

float DistributionGGX() {
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.f);
	float NdotH2 = NdotH * NdotH;

	float numerator = a2;
	float denominator = (NdotH2 * (a2 - 1.f) + 1.f);
	denominator = PI * denominator * denominator;

	return numerator / denominator;
}

float GeometrySchlickGGX(float NdotV) {
	float r = (roughness + 1.f);
	float k = (r * r) / 8.f;

	float numerator = NdotV;
	float denominator = NdotV * (1.f - k) + k;

	return numerator / denominator;
}

float GeometrySmith() {
	float NdotV = max(dot(N, V), 0.f);
	float NdotL = max(dot(N, L), 0.f);
	float ggx2 = GeometrySchlickGGX(NdotV);
	float ggx1 = GeometrySchlickGGX(NdotL);

	return ggx1 * ggx2;
}

vec3 fresnelSchlick(float theta) {
	return F0 + (1.f - F0) * pow(clamp(1.f - theta, 0.f, 1.f), 5.f);
}

vec3 fresnelSchlickRoughness(float theta) {
	return F0 + (max(vec3(1.f - roughness), F0) - F0) * pow(clamp(1.f - theta, 0.f, 1.f), 5.f);
}

float calculateDirectionalShadow() {
	vec4 fragPosViewSpace = view * data_in.fragPos;
	float depthVal = abs(fragPosViewSpace.z);

	int layer = -1;
	for(int i = 0; i < cascadeCount; i++) {
		if(depthVal < cascadePlanes[i]) {
			layer = i;
			break;
		}
	}
	
	if(layer == -1)
		layer = cascadeCount;

	vec4 shadowCoord = lightSpaceMatrices[layer] * data_in.fragPos;
	vec3 projCoords = shadowCoord.xyz / shadowCoord.w;
	projCoords = projCoords * 0.5f + vec3(0.5f);

	float currentDepth = projCoords.z;

	if(currentDepth > 1.f)
		return 1.f;

	ivec3 offsetCoord;
	offsetCoord.xy = ivec2(mod(gl_FragCoord.xy, offsetTexSize.xy));

	float sum = 0.f;
	int samplesDiv2 = int(offsetTexSize.z);
	vec4 sc = vec4(projCoords, 1.f);
	float depth = 0.f;
	vec2 texelSize = 2.f / vec2(textureSize(cascadedShadowMap, 0));

	for(int i = 0; i < 4; i++) {
		offsetCoord.z = i;
		vec4 offsets = texelFetch(randomOffsets, offsetCoord, 0) * radius;

		sc.xy = projCoords.xy + offsets.rg * texelSize;
		depth = texture(cascadedShadowMap, vec3(sc.xy, layer)).r;
		sum += depth > currentDepth ? 1.f : 0.f;

		sc.xy = projCoords.xy + offsets.ba * texelSize;
		depth = texture(cascadedShadowMap, vec3(sc.xy, layer)).r;
		sum += depth > currentDepth ? 1.f : 0.f;
	}

	float shadowFactor = sum / 8.f;

	if(shadowFactor != 1.f && shadowFactor != 0.f) { 
		for(int i = 4; i < samplesDiv2; i++) {
			offsetCoord.z = i;
			vec4 offsets = texelFetch(randomOffsets, offsetCoord, 0) * radius;

			sc.xy = projCoords.xy + offsets.rg * texelSize;
			depth = texture(cascadedShadowMap, vec3(sc.xy, layer)).r;
			sum += depth > currentDepth ? 1.f : 0.f;

			sc.xy = projCoords.xy + offsets.ba * texelSize;
			depth = texture(cascadedShadowMap, vec3(sc.xy, layer)).r;
			sum += depth > currentDepth ? 1.f : 0.f;
		}

		shadowFactor = sum / float(samplesDiv2 * 2.f);
	}

	return shadowFactor;
}

vec4 calculateLighting(Light light, vec3 direction) {
	L = normalize(direction);
	H = normalize(V + L);

	vec3 Lo = vec3(0.f);

	float NDF = DistributionGGX();
	float G = GeometrySmith();
	vec3 F = fresnelSchlick(max(dot(H, V), 0.f));

	vec3 kS = F;
	vec3 kD = 1.f - kS;
	kD *= (1.f - metallic);

	vec3 numerator = NDF * G * F;
	float denominator = 4.f * max(dot(N, V), 0.f) * max(dot(N, L), 0.f) + 0.0001f;
	vec3 specular = numerator / denominator;

	float NdotL = max(dot(N, L), 0.f);
	Lo = (kD * albedo / PI + specular) * light.color * NdotL;

	if(calcShadows) {
		float shadow = calculateDirectionalShadow();
		Lo = shadow * Lo;
	}

	return vec4(Lo, 1.f);
}

float linearizeDepth(float depth) {
	return (2.f * nearPlane * farPlane) / (farPlane + nearPlane - (depth * 2.f - 1.f) * (farPlane - nearPlane));
}

vec4 calcDirectionalLights() {
	return calculateLighting(directionalLight.base, directionalLight.direction);
}

vec4 calcPointLight(PointLight pointLight) {
	vec3 direction = pointLight.position - vec3(data_in.fragPos);
	float dist = length(direction);

	vec4 pointLightColor = calculateLighting(pointLight.base, direction);
	float attenuation = (pointLight.exponent * pow(dist, 2)) + 
						(pointLight.linear * dist) + pointLight.constant;

	return pointLightColor / attenuation;
}

vec4 calcSpotLight(SpotLight spotLight) {
	vec3 direction = normalize(spotLight.base.position - vec3(data_in.fragPos));
	float theta = dot(direction, normalize(-spotLight.direction));
	
	vec4 spotLightColor = vec4(0.f, 0.f, 0.f, 1.f);

	if(theta > spotLight.edge) {
		spotLightColor = calcPointLight(spotLight.base);
		float clampVal = 1.f - ((1.f - theta) * (1.f / (1.f - spotLight.edge)));
		spotLightColor *= clampVal;
	}

	return spotLightColor;
}

vec4 calcPointLights() {
	vec4 totalPointLightColor = vec4(0.f);

	for(int i = 0; i < pointLightCount; i++) {
		totalPointLightColor += calcPointLight(pointLights[i]);
	}

	return totalPointLightColor;
}

vec4 calcSpotLights() {
	vec4 totalSpotLightColor = vec4(0.f);

	for(int i = 0; i < spotLightCount; i++) {
		totalSpotLightColor += calcSpotLight(spotLights[i]);
	}

	return totalSpotLightColor;
}

void main() {
	if(useDiffuseMap) {
		vec4 texColor = texture2D(diffuseMap, data_in.texel);
		if(texColor.a < 0.1f)
			discard;

		albedo = texColor.rgb;
	}

	if(useNormalMap) {
		//texel = parallaxMapping(data_in.texel, eyePosition - vec3(data_in.fragPos));
		N = texture2D(normalMap, data_in.texel).rgb;
		N = normalize(N * 2.f - 1.f);

		if(strippedNormalMap)
			N *= -1.f;
		
		vec3 tangent = normalize(data_in.tangent);
		vec3 normal = normalize(data_in.normal);

		tangent = normalize(tangent - dot(tangent, normal) * normal);
		vec3 biTangent = cross(normal, tangent);

		N = normalize(mat3(tangent, biTangent, normal) * N);
	}

	if(useMaterialMap) {
		ao = 0.3f;
		roughness = texture2D(metallicMap, data_in.texel).g;
		metallic = texture2D(metallicMap, data_in.texel).b;
	}

	F0 = mix(F0, albedo, metallic);

	vec3 R = reflect(-V, N);
	const float MAX_REFLECTION_LOD = 9.f;

	vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.f));

	vec3 kS = F;
	vec3 kD = 1.f - kS;
	kD *= 1.f - metallic;

	vec3 irradiance = texture(irradianceMap, N).rgb;
	vec3 diffuse = irradiance * albedo;

	vec3 prefilteredColor = textureLod(prefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;
	vec2 envBRDF = texture2D(brdfLUT, vec2(max(dot(N, V), 0.f), roughness)).rg;
	vec3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);

	vec3 ambient = (kD * diffuse + specular) * ao;

	vec4 finalColor = calcDirectionalLights() + calcPointLights() + calcSpotLights();

	finalColor += vec4(ambient, 1.f);

	float d = 0.f, mixVal = 0.f;

	if(drawWireframe) {
		d = min(edgeDistance.x, edgeDistance.y);
		d = min(d, edgeDistance.z);

		if(d < wireframeWidth - 1.f)
			mixVal = 1.f;
		else if(d > wireframeWidth + 1.f)
			mixVal = 0.f;
		else {
			float x = d - (wireframeWidth - 1.f);
			mixVal = exp2(-2.f * x * x);
		}

		fragColor = mix(finalColor, wireframeColor, mixVal);
	}
	else
		fragColor = finalColor;
}