#version 330 core
layout (location=0) in vec3 aPos;
layout (location=1) in vec3 aNormal;
layout (location=2) in vec2 aTexcoord;

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

out vec3 worldPos;
out vec3 normal;
out vec2 texcoord;

void main()
{
	vec4 worldPosition = worldMatrix * vec4(aPos, 1.0);
	worldPos = worldPosition.xyz;
	normal = normalize(worldMatrix * vec4(aNormal, 0.0)).xyz;
	texcoord = aTexcoord;
	gl_Position = projMatrix * viewMatrix * worldPosition;
}