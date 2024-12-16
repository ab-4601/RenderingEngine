#version 450 core

in vec3 worldPos;
in vec3 cameraPos;
in float gridSizeVar;

layout (location = 0) out vec4 fragColor;

uniform float gridCellSize = 0.025f;
uniform float minPixelBetweenCells = 2.f;
uniform vec4 gridColorThin = vec4(0.2f, 0.2f, 0.2f, 1.f);
uniform vec4 gridColorThick = vec4(0.5f, 0.5f, 0.5f, 1.f);

float log10(float val) {
	return log(val) / log(10);
}

float satf(float x) {
	return clamp(x, 0.f, 1.f);
}

vec2 satv(vec2 x) {
	return clamp(x, vec2(0.f), vec2(1.f));
}

float max2(vec2 v) {
	return max(v.x, v.y);
}

void main() {
	vec2 dvx = vec2(dFdx(worldPos.x), dFdy(worldPos.x));
	vec2 dvy = vec2(dFdx(worldPos.z), dFdy(worldPos.z));
	
	float lx = length(dvx);
	float ly = length(dvy);

	vec2 dudv = 4.f * vec2(lx, ly);

	float l = length(dudv);

	float LOD = max(0.f, log10(l * minPixelBetweenCells / gridCellSize) + 1.f);

	float gridCellSizeLOD0 = gridCellSize * pow(10, floor(LOD));
	float gridCellSizeLOD1 = gridCellSizeLOD0 * 10;
	float gridCellSizeLOD2 = gridCellSizeLOD1 * 10;

	vec2 modulo = mod(worldPos.xz, gridCellSizeLOD0) / dudv;
	modulo = vec2(1.f) - abs(satv(modulo) * 2.f - vec2(1.f));
	float alpha1 = max2(modulo);

	modulo = mod(worldPos.xz, gridCellSizeLOD1) / dudv;
	modulo = vec2(1.f) - abs(satv(modulo) * 2.f - vec2(1.f));
	float alpha2 = max2(modulo);

	modulo = mod(worldPos.xz, gridCellSizeLOD2) / dudv;
	modulo = vec2(1.f) - abs(satv(modulo) * 2.f - vec2(1.f));
	float alpha3 = max2(modulo);

	float lodFade = fract(LOD);
	vec4 color;

	if(alpha3 > 0.f) {
		color = gridColorThick;
		color.a *= alpha3;
	}
	else {
		if(alpha2 > 0.f) {
			color = mix(gridColorThick, gridColorThin, lodFade);
			color.a *= alpha2;
		}
		else {
			color = gridColorThin;
			color.a *= (alpha1 * (1.f - lodFade));
		}
	}

	float opacityFalloff = 1 - satf(length(worldPos.xz - cameraPos.xz) / gridSizeVar);

	color.a *= opacityFalloff;

	fragColor = vec4(color);
}