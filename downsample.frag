 # version 450 core

layout (location = 0) out vec3 downsample;

uniform sampler2D srcTexture;
uniform vec2 srcResolution;
uniform int mipLevel = 1;

in vec2 texel;

vec3 powVec3(vec3 v, float p) {
    return vec3(pow(v.x, p), pow(v.y, p), pow(v.z, p));
}

float RGBToLuminance(vec3 col) {
    return dot(col, vec3(0.2126f, 0.7152f, 0.0722f));
}

vec3 srgb(vec3 v) {
    return powVec3(v, 1 / 2.2f);
}

float KarisAverage(vec3 col) {
    float luma = RGBToLuminance(srgb(col)) * 0.25f;
    return 1.f / (1.f + luma);
}

void main() {
	vec2 srcTexelSize = 1.f / srcResolution;

	float x = srcTexelSize.x;
	float y = srcTexelSize.y;

	vec3 a = texture(srcTexture, vec2(texel.x - 2 * x, texel.y + 2 * y)).rgb;
    vec3 b = texture(srcTexture, vec2(texel.x,       texel.y + 2 * y)).rgb;
    vec3 c = texture(srcTexture, vec2(texel.x + 2 * x, texel.y + 2 * y)).rgb;

    vec3 d = texture(srcTexture, vec2(texel.x - 2 * x, texel.y)).rgb;
    vec3 e = texture(srcTexture, vec2(texel.x,       texel.y)).rgb;
    vec3 f = texture(srcTexture, vec2(texel.x + 2 * x, texel.y)).rgb;

    vec3 g = texture(srcTexture, vec2(texel.x - 2 * x, texel.y - 2 * y)).rgb;
    vec3 h = texture(srcTexture, vec2(texel.x,       texel.y - 2 * y)).rgb;
    vec3 i = texture(srcTexture, vec2(texel.x + 2 * x, texel.y - 2 * y)).rgb;

    vec3 j = texture(srcTexture, vec2(texel.x - x, texel.y + y)).rgb;
    vec3 k = texture(srcTexture, vec2(texel.x + x, texel.y + y)).rgb;
    vec3 l = texture(srcTexture, vec2(texel.x - x, texel.y - y)).rgb;
    vec3 m = texture(srcTexture, vec2(texel.x + x, texel.y - y)).rgb;

    vec3 groups[5];

	switch (mipLevel)
	{
		case 0:
			groups[0] = (a + b + d + e) * (0.125f / 4.0f);
			groups[1] = (b + c + e + f) * (0.125f / 4.0f);
			groups[2] = (d + e + g + h) * (0.125f / 4.0f);
			groups[3] = (e + f + h + i) * (0.125f / 4.0f);
			groups[4] = (j + k + l + m) * (0.5f / 4.0f);
			groups[0] *= KarisAverage(groups[0]);
			groups[1] *= KarisAverage(groups[1]);
			groups[2] *= KarisAverage(groups[2]);
			groups[3] *= KarisAverage(groups[3]);
			groups[4] *= KarisAverage(groups[4]);
			downsample = groups[0] + groups[1] + groups[2] + groups[3] + groups[4];
			break;

		default:
			downsample = e * 0.125f;               
			downsample += (a + c + g + i) * 0.03125f;    
			downsample += (b + d + f + h) * 0.0625f;     
			downsample += (j + k + l + m) * 0.125f;      
			break;
	}

	downsample = max(downsample, 0.0001f);
}