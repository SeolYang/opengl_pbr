#version 450 core
layout (location=0) in vec3 aPos;

uniform mat4 shadowViewMatrix;
uniform mat4 shadowProjMatrix;
uniform mat4 worldMatrix;

void main()
{
   gl_Position = shadowProjMatrix*shadowViewMatrix*worldMatrix*vec4(aPos, 1.0f);
}