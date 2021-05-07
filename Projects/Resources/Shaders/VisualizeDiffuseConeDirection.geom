#version 450 core
layout(triangles) in;
layout(line_strip, max_vertices = 14) out;

in vec3 worldPosGeom[];
in vec3 worldNormalGeom[];
in vec3 worldTangentGeom[];
in mat3 tbnGeom[];
in mat3 tnbGeom[];

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform float directionLength = 1.5f;
uniform int onlyNormal = 0;

out vec3 coneDirFrag;

const int NumOfCones = 6;
vec3 coneDirections[6] = vec3[](
	vec3(0.0, 1.0, 0.0),
	vec3(0.0, 0.5, 0.866025),
	vec3(0.823639, 0.5, 0.267617),
	vec3(0.509037, 0.5, -0.700629),
	vec3(-0.509037, 0.5, -0.700629),
	vec3(-0.823639, 0.5, 0.267617));

void main()
{
    vec3 center = (worldPosGeom[0]+worldPosGeom[1]+worldPosGeom[2])/3.0;
	vec3 normal = (worldNormalGeom[0]+worldNormalGeom[1]+worldNormalGeom[2])/3.0;

	if (onlyNormal != 1)
	{
		for(int cone = 0; cone < 6; ++cone)
		{
			vec3 direction = normalize(tnbGeom[0] * coneDirections[cone]);
			vec3 p0 = center;
			vec3 p1 = p0 + (directionLength * direction);

			coneDirFrag = direction * 0.5f + vec3(0.5f);
			gl_Position = projMatrix * viewMatrix * vec4(p0, 1.0);
			EmitVertex();
			gl_Position = projMatrix * viewMatrix * vec4(p1, 1.0);
			EmitVertex();
			EndPrimitive();
		}
	}

	coneDirFrag = vec3(1.0f);
	gl_Position = projMatrix * viewMatrix * vec4(center, 1.0);
	EmitVertex();
	gl_Position = projMatrix * viewMatrix * vec4(center+normal*directionLength*2.0, 1.0);
	EmitVertex();
	EndPrimitive();
}