#version 450 core

const float PI = 3.14159265359;

out vec4 fragColor;

in vec3 texel;

uniform samplerCube environmentMap;

mat3 aces_input_matrix = mat3(
	vec3(0.59719f, 0.35458f, 0.04823f),
    vec3(0.07600f, 0.90834f, 0.01566f),
    vec3(0.02840f, 0.13383f, 0.83777f)
);

mat3 aces_output_matrix = mat3(
	vec3( 1.60475f, -0.53108f, -0.07367f),
    vec3(-0.10208f,  1.10813f, -0.00605f),
    vec3(-0.00327f, -0.07276f,  1.07602f)
);

vec3 mul(const mat3 m, vec3 v) {
	float x = m[0][0] * v[0] + m[0][1] * v[1] + m[0][2] * v[2];
    float y = m[1][0] * v[0] + m[1][1] * v[1] + m[1][2] * v[2];
    float z = m[2][0] * v[0] + m[2][1] * v[1] + m[2][2] * v[2];

    return vec3(x, y, z);
}

vec3 rtt_and_odt_fit(vec3 v) {
	vec3 a = v * (v + 0.0245786f) - 0.000090537f;
    vec3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
    return a / b;
}

vec3 aces(vec3 color) {
	color = mul(aces_input_matrix, color);
    color = rtt_and_odt_fit(color);
    return mul(aces_output_matrix, color);
}

void main() {
	vec3 normal = normalize(texel);
	vec3 irradiance = vec3(0.f);

	vec3 up = vec3(0.f, 1.f, 0.f);
	vec3 right = normalize(cross(up, normal));
	up = normalize(cross(normal, right));

	float delta = 0.025f;
	float samples = 0.f;

	for(float phi = 0.f; phi < 2.f * PI; phi += delta) {
		for(float theta = 0.f; theta < 0.5f * PI; theta += delta) {
			vec3 tangent = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
			vec3 sampleVec = tangent.x * right + tangent.y * up + tangent.z * normal;

			irradiance += aces(texture(environmentMap, sampleVec).rgb) * cos(theta) * sin(theta);
			samples++;
		}
	}

	irradiance = PI * irradiance * (1.f / samples);

	fragColor = vec4(irradiance, 1.f);
}