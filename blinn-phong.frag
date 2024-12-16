#version 450 core

layout (location = 0) out vec4 fragColor;

in GEOM_DATA {
	vec4 vColor;
    vec2 texel;
    vec3 normal;
	vec3 tangent;
    vec4 fragPos;
	vec4 lightSpaceFragPos;
} data_in;

const int MAX_POINT_LIGHTS = 3;
const int MAX_SPOT_LIGHTS = 3;

struct Light {
	vec3 color;
	float ambientIntensity;
	float diffuseIntensity;
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
	float specularIntensity;
	float specularPower;
};

uniform bool useTexture = false;
uniform bool useNormalMap = false;
uniform bool calcShadows = false;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;
uniform sampler2D directionalShadowMap;
uniform samplerCube pointShadowMap;

uniform float farPlane;

uniform int pointLightCount;
uniform int spotLightCount;

uniform DirectionalLight directionalLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];
uniform Material material;

uniform vec3 eyePosition;
uniform float height_scale;

//const float minLayers = 8.f;
//const float maxLayers = 32.f;

//vec2 parallaxMapping(vec2 texel, vec3 viewDir) {
	//float numLayers = mix(maxLayers, minLayers, max(dot(vec3(0.f, 0.f, 1.f), viewDir), 0.f));
	//float layerDepth = 1.f / numLayers;

	//float currentLayerDepth = 0.f;

	//vec2 P = viewDir.xy * height_scale;
	//vec2 deltaTexel = P / numLayers;

	//vec2 currTexel = data_in.texel;
	//float currDepth = texture(depthMap, currTexel).r;

	//while(currentLayerDepth < currDepth) {
		//currTexel -= deltaTexel;
		//currDepth = texture(depthMap, currTexel).r;
		//currentLayerDepth += layerDepth;
	//}

	//return currTexel;
//}

float rand(vec2 v){
    return fract(sin(dot(v, vec2(12.9898, 78.233))) * 43758.5453);
}

const vec2 poissonSamples[] = vec2[](
    vec2( 0.334,  0.334), vec2( 0.557,  0.334), vec2( 0.778,  0.334), vec2( 1.0,  0.334),
    vec2( 0.334,  0.557), vec2( 0.557,  0.557), vec2( 0.778,  0.557), vec2( 1.0,  0.557),
    vec2( 0.334,  0.778), vec2( 0.557,  0.778), vec2( 0.778,  0.778), vec2( 1.0,  0.778),
    vec2( 0.334,  1.0),   vec2( 0.557,  1.0),   vec2( 0.778,  1.0),   vec2( 1.0,  1.0),
    vec2( 0.25,   0.25),  vec2( 0.5,    0.25),  vec2( 0.75,   0.25),  vec2( 1.0,  0.25),
    vec2( 0.25,   0.5),   vec2( 0.5,    0.5),   vec2( 0.75,   0.5),   vec2( 1.0,  0.5),
    vec2( 0.25,   0.75),  vec2( 0.5,    0.75),  vec2( 0.75,   0.75),  vec2( 1.0,  0.75)
);

float calculateDirectionalShadow() {
	vec3 projectionCoords = data_in.lightSpaceFragPos.xyz / data_in.lightSpaceFragPos.w;
	projectionCoords = projectionCoords * 0.5f + 0.5f;

	float closestDepth = texture2D(directionalShadowMap, projectionCoords.xy).r;
	float currentDepth = projectionCoords.z;

	float shadow = 0.f;
	float shadowSamples = 16.f;
	float shadowThreshold = 0.6f;

	float shadowMapVal = texture(directionalShadowMap, projectionCoords.xy).r;
	float lightSpaceFragDepth = data_in.lightSpaceFragPos.z;

	// PCF with random sampling
	if(abs(lightSpaceFragDepth - shadowMapVal) < shadowThreshold) {
		for(float i = 0.f; i < shadowSamples; i++) {
			vec2 randomOffset = vec2(
				rand(projectionCoords.xy + i) - 0.5f,
				rand(projectionCoords.xy + i * 0.5f) - 0.5f
			);

			randomOffset *= 0.005f;

			float sampleDepth = texture(directionalShadowMap, projectionCoords.xy + randomOffset).r;

			shadow += currentDepth > sampleDepth ? 1.f : 0.f;
		}
	}

	shadow /= shadowSamples;

	return shadow;
}

vec4 calcLightByDirection(Light light, vec3 direction, vec3 normal) {
	vec3 ambientColor = light.color * light.ambientIntensity;

	float diffuseFactor = max(dot(normalize(normal), normalize(direction)), 0.f);
	vec3 diffuseColor = light.color * light.diffuseIntensity * diffuseFactor;

	vec3 specularColor = vec3(0.f, 0.f, 0.f);

	if(diffuseFactor > 0.f) {
		vec3 fragToEye = normalize(eyePosition - vec3(data_in.fragPos));
		//halfwayDir = normalize(normalize(direction) + fragToEye);
		//float specularFactor = dot(normalize(normal), halfwayDir);

		vec3 reflection = reflect(-normalize(direction), normalize(normal));
		float specularFactor = dot(normalize(fragToEye), normalize(reflection));
		
		if(specularFactor > 0.f) {
			specularFactor = pow(specularFactor, material.specularPower);
			specularColor = light.color * material.specularIntensity * specularFactor;
		}
	}

	vec3 lighting = vec3(0.f);

	if(calcShadows) {
		float shadow = calculateDirectionalShadow();
		lighting = ambientColor + (1.f - shadow) * (diffuseColor + specularColor);
	}
	else
		lighting = ambientColor + diffuseColor + specularColor;

	return vec4(lighting, 1.f);
}

vec4 calcDirectionalLight(vec3 normal) {
	return calcLightByDirection(directionalLight.base, directionalLight.direction, normal);
}

vec4 calcPointLight(PointLight pointLight, vec3 normal) {
	vec3 direction = pointLight.position - vec3(data_in.fragPos);
	float dist = length(direction);
	direction = normalize(direction);

	vec4 currColor = calcLightByDirection(pointLight.base, direction, normal);
	float attenuation = (pointLight.exponent * pow(dist, 2)) + 
						(pointLight.linear * dist) + pointLight.constant;

	return currColor / attenuation;
}

vec4 calcPointLights(vec3 normal) {
	vec4 totalPointLightColor = vec4(0.f, 0.f, 0.f, 1.f);

	for(int i = 0; i < pointLightCount; i++)
		totalPointLightColor += calcPointLight(pointLights[i], normal);

	return totalPointLightColor;
}

vec4 calcSpotLight(SpotLight spotLight, vec3 normal) {
	vec3 direction = normalize(spotLight.base.position - vec3(data_in.fragPos));
	float theta = dot(direction, normalize(-spotLight.direction));
	
	vec4 spotLightColor = vec4(0.f, 0.f, 0.f, 1.f);

	if(theta > spotLight.edge) {
		spotLightColor = calcPointLight(spotLight.base, normal);
		float clampVal = 1.f - ((1.f - theta) * (1.f/(1.f - spotLight.edge)));
		spotLightColor *= clampVal;
	}

	return spotLightColor;
}

vec4 calcSpotLights(vec3 normal) {
	vec4 finalSpotLightColor = vec4(0.f, 0.f, 0.f, 1.f);

	for(int i = 0; i < spotLightCount; i++) {
		finalSpotLightColor += calcSpotLight(spotLights[i], normal);
	}

	return finalSpotLightColor;
}

void main() {
	vec3 normal = data_in.normal;
	//vec2 texel = data_in.texel;

	if(useNormalMap) {
		//texel = parallaxMapping(data_in.texel, eyePosition - vec3(data_in.fragPos));
		normal = texture(normalMap, data_in.texel).rgb;
		normal = normalize(normal * 2.f - 1.f);
		
		vec3 T = normalize(data_in.tangent);
		vec3 N = normalize(data_in.normal);

		T = normalize(T - dot(T, N) * N);
		vec3 B = cross(N, T);

		normal = normalize(mat3(T, B, N) * normal);
	}

	vec4 finalColor = calcDirectionalLight(normal) + calcPointLights(normal) + calcSpotLights(normal);

	if (!useTexture) {
		finalColor *= data_in.vColor;
	}
	else {
		vec4 texColor = texture(diffuseMap, data_in.texel);

		if(texColor.a < 0.1)
			discard;

		finalColor *= texture(diffuseMap, data_in.texel);
	}

	fragColor = finalColor;
}