#version 330 core
out vec4 FragColor;

in vec3 Position;
in vec3 Normal;
in vec2 Texcoord;

uniform vec3 LightPosition;

void main()
{
	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(LightPosition - Position);
	FragColor = vec4(vec3(1.0, 1.0, 1.0)*dot(lightDir, normal), 1.0);
}