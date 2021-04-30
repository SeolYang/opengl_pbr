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

void main()
{
	vec4 worldPosition = worldMatrix * vec4(aPos, 1.0);
	worldPosFrag = worldPosition.xyz;
	texCoordsFrag = aTexcoord;

	vec3 normal = normalize(mat3(transpose(inverse(worldMatrix))) * aNormal.xyz);
	worldNormalFrag = normal;

	vec3 tangent = normalize(worldMatrix * vec4(aTangent, 0.0)).xyz;
	tangent = normalize(tangent - dot(tangent, normal) * normal);
	vec3 bitangent = normalize(cross(normal, tangent).xyz);

	tbnFrag = mat3(tangent, bitangent, normal);

	shadowPosFrag = shadowProjMat * shadowViewMat * worldPosition;
	shadowPosFrag.xyz = shadowPosFrag.xyz * 0.5f + vec3(0.5f);

	gl_Position = projMatrix * viewMatrix * worldPosition;
}