#version 450 core
layout(location = 0) in vec3 aPos;
out vec2 texCoordsFrag;

vec2 Scale(vec2 v)
{
   return 0.5 * v + vec2(0.5);
}

void main()
{
   texCoordsFrag = Scale(aPos.xy);
   gl_Position = vec4(aPos, 1.0);
}