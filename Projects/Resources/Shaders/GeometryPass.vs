#version 330 core
layout (location=0) in vec3 aPos;
layout (location=1) in vec2 aTexcoord;
layout (location=2) in vec3 aNormal;
layout (location=3) in vec3 aTangent;

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

out vec3 worldPos;
out vec2 texcoord;
out vec3 worldNormal;
out mat3 tbn;

void main()
{
	vec4 worldPosition = worldMatrix * vec4(aPos, 1.0);
	worldPos = worldPosition.xyz;
	texcoord = aTexcoord;

	vec3 normal = normalize(mat3(transpose(inverse(worldMatrix)))*aNormal.xyz);
	worldNormal = normal;

	vec3 tangent = normalize(worldMatrix * vec4(aTangent, 0.0)).xyz;
	tangent = normalize(tangent - dot(tangent, normal) * normal);
	vec3 bitangent = normalize(cross(normal, tangent).xyz);

	tbn = mat3(tangent, bitangent, normal);

	gl_Position = projMatrix * viewMatrix * worldPosition;
}