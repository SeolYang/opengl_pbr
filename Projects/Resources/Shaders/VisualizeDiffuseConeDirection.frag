#version 450 core
in vec3 coneDirFrag;

out vec4 fragColor;

void main() 
{
    fragColor = vec4(coneDirFrag, 1.0f);
}