#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexcoord;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec3 aTangent;

uniform mat4 worldMatrix;

out vec3 worldPosGeom;
out vec2 texCoordsGeom;
out vec3 worldNormalGeom;
out mat3 tbnGeom;

void main()
{
	vec4 worldPosition = worldMatrix * vec4(aPos, 1.0);
	worldPosGeom = worldPosition.xyz;
	texCoordsGeom = aTexcoord;

	vec3 normal = normalize(mat3(transpose(inverse(worldMatrix))) * aNormal.xyz);
	worldNormalGeom = normal;

	vec3 tangent = normalize(worldMatrix * vec4(aTangent, 0.0)).xyz;
	tangent = normalize(tangent - dot(tangent, normal) * normal);
	vec3 bitangent = normalize(cross(normal, tangent).xyz);

	tbnGeom = mat3(tangent, bitangent, normal);

	gl_Position = worldPosition;
}