#version 450 core

layout ( triangles ) in;
layout ( triangle_strip, max_vertices = 3 ) out;

in VERT_DATA {
	vec4 color;
    vec2 texel;
    vec3 normal;
	vec3 tangent;
    vec4 fragPos;
} data_in[];

out GEOM_DATA {
	vec4 color;
    vec2 texel;
    vec3 normal;
	vec3 tangent;
    vec4 fragPos;
} data_out;

noperspective out vec3 edgeDistance;

uniform mat4 viewportMatrix;
uniform bool drawWireframe;

void main() {
	vec3 edgeDistances[3] = vec3[](vec3(0.f), vec3(0.f), vec3(0.f));

	if(drawWireframe) {
		vec4 p = vec4(0.f);

		p = gl_in[0].gl_Position;
		vec2 p0 = vec2(viewportMatrix * (p / p.w));

		p = gl_in[1].gl_Position;
		vec2 p1 = vec2(viewportMatrix * (p / p.w));

		p = gl_in[2].gl_Position;
		vec2 p2 = vec2(viewportMatrix * (p / p.w));

		float a = length(p1 - p2);
		float b = length(p2 - p0);
		float c = length(p1 - p0);

		float alpha = acos((b * b + c * c - a * a) / (2.f * b * c));
		float beta = acos((a * a + c * c - b * b) / (2.f * a * c));

		float ha = abs(c * sin(beta));
		float hb = abs(c * sin(alpha));
		float hc = abs(b * sin(alpha));

		edgeDistances[0] = vec3(ha, 0.f, 0.f);
		edgeDistances[1] = vec3(0.f, hb, 0.f);
		edgeDistances[2] = vec3(0.f, 0.f, hc);
	}

	for(int i = 0; i < 3; i++) {
		data_out.color = data_in[i].color;
		data_out.texel = data_in[i].texel;
		data_out.normal = data_in[i].normal;
		data_out.tangent = data_in[i].tangent;
		data_out.fragPos = data_in[i].fragPos;
		edgeDistance = edgeDistances[i];
		gl_Position = gl_in[i].gl_Position;
		EmitVertex();
	}

	EndPrimitive();
}