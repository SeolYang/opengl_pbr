#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexcoord;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec3 aTangent;

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

out vec3 worldPos;

void main()
{
   worldPos = vec3(worldMatrix * vec4(aPos, 1.0));
   gl_Position = projMatrix * viewMatrix * vec4(worldPos, 1.0);
}