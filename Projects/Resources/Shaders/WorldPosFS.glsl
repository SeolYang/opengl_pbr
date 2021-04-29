#version 450 core
in vec3 worldPos;
out vec3 color;

void main()
{
   color.rgb = worldPos;
}