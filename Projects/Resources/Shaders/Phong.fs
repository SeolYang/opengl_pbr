// https://github.com/KhronosGroup/glTF/tree/master/specification/2.0
#version 330 core
out vec4 fragColor;

const int maxLightsNum = 32;

struct Light{
	vec3 position;
	vec3 radiance;
};

in vec3 worldPos;
in vec2 texcoord;
in vec3 worldNormal;
in mat3 tbn;

uniform Light lights[maxLightsNum];
uniform int numOfLights;

// baseColorMap: sRGB
uniform sampler2D baseColorMap;
uniform vec4 baseColorFactor;

uniform sampler2D normalMap;

uniform sampler2D metallicRoughnessMap;
uniform float metallicFactor;
uniform float roughnessFactor;

// aoMap(Ambient Occlusion Map): Linear(R channel only)
uniform sampler2D aoMap;

// emissiveMap: sRGB
uniform sampler2D emissiveMap;
uniform vec3 emissiveFactor;

uniform vec3 camPos;
uniform vec3 lightPos;

uniform int bUseNormalMap;

vec3 pow3(vec3 v, float power)
{
	return vec3(pow(v.x, power), pow(v.y, power), pow(v.z, power));
}

void main()
{
	float ao = texture(aoMap, texcoord).r;

	vec3 albedo = pow3(texture(baseColorMap, texcoord).rgb, 2.2);
	albedo += pow3(baseColorFactor.rgb, 2.2);

	vec3 normal = worldNormal;
	if (bUseNormalMap == 1)
	{
		normal = texture(normalMap, texcoord).rgb;
		normal = normalize(normal*2.0-1.0);
		normal = normalize(tbn * normal);
	}
	
	vec3 N = normalize(normal);
	vec3 V = normalize(camPos - worldPos);

	vec3 Lo = vec3(0.0);

	for(int idx = 0; idx < numOfLights; ++idx)
	{
		vec3 L = normalize(lights[idx].position - worldPos);
		float NdotL = max(dot(N, L), 0.0);
		vec3 reflected = reflect(L, N);
		float VdotR = pow(max(dot(reflected, V), 0.0), 32.0);

		float distance = length(lights[idx].position - worldPos);
		float attenuation = 1.0/(distance*distance);

		Lo += attenuation*((lights[idx].radiance*NdotL)*albedo + (vec3(1.0)*VdotR));
	}

	vec3 ambient = vec3(0.03)*albedo*ao;
	vec3 emissive = pow3(texture(emissiveMap, texcoord).rgb, 2.2);
	emissive += pow3(emissiveFactor, 2.2);

	vec3 color = ambient + emissive + Lo;

	color = color/(color+vec3(1.0));
	color = pow(color, vec3(1.0/2.2));

	fragColor = vec4(color, 1.0);
}