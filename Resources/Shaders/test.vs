#version 330 core
layout (location=0) in vec3 aPos;
layout (location=1) in vec3 aNormal;
layout (location=2) in vec2 aTexcoord;

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

out vec3 Position;
out vec3 Normal;
out vec2 Texcoord;

void main()
{
	vec4 worldPosition = worldMatrix * vec4(aPos, 1.0);
	Position = worldPosition.xyz;
	gl_Position = projMatrix * viewMatrix * worldPosition;
	Normal = aNormal;
	Texcoord = aTexcoord;
}