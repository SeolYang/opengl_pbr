#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexcoord;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec3 aTangent;

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 shadowViewMat;
uniform mat4 shadowProjMat;

out vec3 worldPosFrag;
out vec4 shadowPosFrag;
out vec2 texCoordsFrag;
out vec3 worldNormalFrag;
out mat3 tbnFrag;
out mat3 tnbFrag;

void main()
{
	vec4 worldPosition = worldMatrix * vec4(aPos, 1.0);
	worldPosFrag = worldPosition.xyz;
	texCoordsFrag = aTexcoord;

	mat3 normalMatrix = mat3(transpose(inverse(worldMatrix)));
	vec3 normal = normalize(normalMatrix * aNormal.xyz);
	worldNormalFrag = normal;

	vec3 tangent = normalize(normalMatrix * aTangent.xyz).xyz;
	tangent = normalize(tangent - dot(tangent, normal) * normal); // Gram-Schmit 
	vec3 bitangent = normalize(cross(normal, tangent).xyz);

	tbnFrag = mat3(tangent, bitangent, normal);
	tnbFrag = mat3(tangent, normal, bitangent);

	shadowPosFrag = shadowProjMat * shadowViewMat * worldPosition;
	shadowPosFrag.xyz = shadowPosFrag.xyz * 0.5f + vec3(0.5f);

	gl_Position = projMatrix * viewMatrix * worldPosition;
}