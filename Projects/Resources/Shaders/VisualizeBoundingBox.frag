#version 450 core
uniform vec3 boundingBoxColor = vec3(0.0f, 1.0f, 0.0f);

out vec4 fragColor;

void main()
{
    fragColor = vec4(boundingBoxColor, 1.0f);
}