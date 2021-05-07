#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexcoord;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec3 aTangent;

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

out vec3 worldPosGeom;
out vec3 worldNormalGeom;
out vec3 worldTangentGeom;
out mat3 tbnGeom;
out mat3 tnbGeom;

void main()
{
	vec4 worldPosition = worldMatrix * vec4(aPos, 1.0);
	worldPosGeom = worldPosition.xyz;

	mat3 normalMatrix = mat3(transpose(inverse(worldMatrix)));
	vec3 normal = normalize(normalMatrix * aNormal.xyz);
	worldNormalGeom = normal;

	const float epsilon = 0.0001;
	vec3 tangent = vec3(0.0f);
	vec3 bitangent = vec3(0.0f);
	if (length(aTangent) - epsilon > 0.0)
	{
		tangent = normalize(normalMatrix * aTangent.xyz).xyz;
		tangent = normalize(tangent - dot(tangent, normal) * normal); // Gram-Schmit 
		bitangent = normalize(cross(normal, tangent).xyz);
	}
	else
	{
		vec3 up = abs(normal.z) < 0.999f ? vec3(0.0f, 0.0f, 1.0f) : vec3(1.0f, 0.0f, 0.0f);
		tangent = normalize(cross(up, normal));
		bitangent = cross(normal, tangent);
	}

	tbnGeom = mat3(tangent, bitangent, normal);
	tnbGeom = mat3(tangent, normal, bitangent);

	gl_Position = projMatrix * viewMatrix * worldPosition;
}