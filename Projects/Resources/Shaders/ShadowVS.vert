#version 450 core
layout (location=0) in vec3 aPos;

uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 worldMatrix;

void main()
{
   gl_Position = projMatrix*viewMatrix*worldMatrix*vec4(aPos, 1.0f);
}