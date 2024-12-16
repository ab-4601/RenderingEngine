#version 450 core

out vec4 fragColor;

in GS_OUT {
	vec4 color;
	vec2 texCoord1;
	vec2 texCoord2;
	float blend;
} fs_in;

uniform sampler2D textureSampler;

void main() {
	//fragColor = fs_in.color;
	vec4 texColor1 = texture2D(textureSampler, fs_in.texCoord1);
	vec4 texColor2 = texture2D(textureSampler, fs_in.texCoord2);

	vec4 finalColor = mix(texColor1, texColor2, fs_in.blend);

	if(finalColor.a < 0.1)
		discard;

	fragColor = pow(finalColor * 3.f, vec4(2.2));
}