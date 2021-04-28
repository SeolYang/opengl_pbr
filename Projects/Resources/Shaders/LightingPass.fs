#version 330 core
out vec4 fragColor;

const int maxLightsNum = 32;

struct Light{
	vec3 position;
	vec3 radiance;
};

in vec2 texcoord;

uniform Light lights[maxLightsNum];
uniform int numOfLights;

uniform sampler2D positionBuffer;
uniform sampler2D normalBuffer;
uniform sampler2D albedoBuffer;
uniform sampler2D metallicRoughnessBuffer;
uniform sampler2D emissiveAOBuffer;

uniform vec3 camPos;

const float PI = 3.14159265359;

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness*roughness;
	float a2 = a*a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH*NdotH;

	float nom = a2;
	float denom = (NdotH2 * (a2-1.0) + 1.0);
	denom = PI * denom * denom;

	return nom/max(denom, 0.001);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;

	float nom = NdotV;
	float denom = NdotV * (1.0-k) + k;

	return nom/denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1*ggx2;
}

vec3 pow3(vec3 v, float power)
{
	return vec3(pow(v.x, power), pow(v.y, power), pow(v.z, power));
}

void main()
{
	vec4 emissiveAO = texture(emissiveAOBuffer, texcoord);
	float ao = emissiveAO.a;

	vec3 worldPos = texture(positionBuffer, texcoord).xyz;
	vec3 albedo = texture(albedoBuffer, texcoord).rgb;
	vec3 normal = texture(normalBuffer, texcoord).xyz;
	
	vec3 N = normalize(normal);
	vec3 V = normalize(camPos - worldPos);

	vec3 Lo = vec3(0.0);
	
	float metallic = texture(metallicRoughnessBuffer, texcoord).r;
	float roughness = texture(metallicRoughnessBuffer, texcoord).g;

	for(int idx = 0; idx < numOfLights; ++idx)
	{
		vec3 L = normalize(lights[idx].position - worldPos);
		vec3 H = normalize(V + L);

		float NdotL = max(dot(N, L), 0.0);

		float distance = length(lights[idx].position - worldPos);
		float attenuation = 1.0/(distance*distance);
		
		vec3 radiance = lights[idx].radiance*attenuation;

		vec3 F0 = vec3(0.04);
		F0 = mix(F0, albedo, metallic);
		vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

		float NDF = DistributionGGX(N, H, roughness);
		float G = GeometrySmith(N, V, L, roughness);

		vec3 nominator = NDF*G*F;
		float denominator = 4.0*max(dot(N,V), 0.0) * max(dot(N, L), 0.0);
		vec3 specular = nominator/max(denominator, 0.001);

		vec3 kS = F;
		vec3 kD = vec3(1.0)-kS;

		kD *= 1.0-metallic;

		Lo += ((kD * albedo / PI) + specular) * radiance * NdotL;
	}

	vec3 ambient = vec3(0.03)*albedo*ao;
	vec3 emissive = emissiveAO.rgb;

	vec3 color = ambient + emissive + Lo;

	color = color/(color+vec3(1.0));
	color = pow(color, vec3(1.0/2.2));

	fragColor = vec4(color, 1.0);
}