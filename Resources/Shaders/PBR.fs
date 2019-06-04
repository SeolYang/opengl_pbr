// https://github.com/KhronosGroup/glTF/tree/master/specification/2.0
#version 330 core
out vec4 fragColor;

in vec3 worldPos;
in vec2 texcoord;
in vec3 worldNormal;
in mat3 tbn;

// baseColorMap: sRGB
uniform sampler2D baseColorMap;
uniform vec4 baseColorFactor;

uniform sampler2D normalMap;

// metallicRoughnessMap: Linear(B:Metallic, G:Roughness)
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

	float num = a2;
	float denom = (NdotH2 * (a2-1.0) + 1.0);
	denom = PI * denom * denom;

	return num/denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;

	float num = NdotV;
	float denom = NdotV * (1.0-k) + k;

	return num/denom;
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
	//vec3 albedo = baseColorFactor.rgb + texture(baseColor, texcoord).rgb;
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

	vec3 L = normalize(lightPos - worldPos);
	vec3 H = normalize(V + L);

	float NdotL = max(dot(N, L), 0.0);

	float distance = length(lightPos - worldPos);
	float attenuation = 1.0/(distance*distance);
	// assume white light
	vec3 radiance = vec3(40.0) * attenuation;

	float metallic = metallicFactor + texture(metallicRoughnessMap, texcoord).b;
	float roughness = roughnessFactor + texture(metallicRoughnessMap, texcoord).g;

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);
	vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

	float NDF = DistributionGGX(N, H, roughness);
	float G = GeometrySmith(N, V, L, roughness);

	vec3 numerator = NDF*G*F;
	float denominator = 4.0*max(dot(N,V), 0.0) * max(dot(N, L), 0.0);
	vec3 specular = numerator/max(denominator, 0.001);

	vec3 kS = F;
	vec3 kD = vec3(1.0)-kS;

	kD *= 1.0-metallic;

	Lo += ((kD * albedo / PI) + specular) * radiance * NdotL;

	vec3 ambient = vec3(0.03)*albedo*ao;
	vec3 emissive = pow3(texture(emissiveMap, texcoord).rgb, 2.2);
	emissive += pow3(emissiveFactor, 2.2);

	vec3 color = ambient + emissive + Lo;

	color = color/(color+vec3(1.0));
	color = pow(color, vec3(1.0/2.2));

	fragColor = vec4(color, 1.0);
}