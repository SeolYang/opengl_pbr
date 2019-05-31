#version 330 core
out vec4 FragColor;

in vec3 Position;
in vec3 Normal;
in vec2 Texcoord;

uniform vec3 LightPosition;

uniform sampler2D baseColor;
uniform vec4 baseColorFactor;

void main()
{
	vec3 color = texture(baseColor, Texcoord).rgb + baseColorFactor.rgb;
	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(LightPosition - Position);

	float ambientFactor = 0.1;
	vec3 ambient = ambientFactor*color;

	float diffuseFactor = clamp(dot(lightDir, normal), 0.0, 1.0);
	vec3 diffuse = diffuseFactor*color;

	FragColor = vec4(ambient+diffuse, 1.0);
}