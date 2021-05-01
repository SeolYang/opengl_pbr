#version 330 core
out vec4 fragColor;
layout (location=0) out vec3 aPosition;
layout (location=1) out vec3 aNormal;
layout (location=2) out vec4 aAlbedo;
layout (location=3) out vec2 aMetallicRoughness;
layout (location=4) out vec4 aEmissiveAO;

in vec3 worldPos;
in vec2 texcoord;
in vec3 worldNormal;
in mat3 tbn;

// baseColorMap: sRGB
uniform sampler2D baseColorMap;
uniform vec4 baseColorFactor;

uniform sampler2D normalMap;
uniform int bUseNormalMap;

// metallicRoughnessMap: Linear(B:Metallic, G:Roughness)
uniform sampler2D metallicRoughnessMap;
uniform float metallicFactor;
uniform float roughnessFactor;

// aoMap(Ambient Occlusion Map): Linear(R channel only)
uniform sampler2D aoMap;

// emissiveMap: sRGB
uniform sampler2D emissiveMap;
uniform vec3 emissiveFactor;
uniform float emissiveIntensity;


const float PI = 3.14159265359;

vec3 pow3(vec3 v, float power)
{
	return vec3(pow(v.x, power), pow(v.y, power), pow(v.z, power));
}

void main()
{
	float ao = texture(aoMap, texcoord).r;

	vec4 albedo = texture(baseColorMap, texcoord);
	if (albedo.a < 0.1)
	{
		discard;
	}
	albedo = vec4(pow3(albedo.rgb, 2.2) + pow3(baseColorFactor.rgb, 2.2), albedo.a);

	vec3 normal = worldNormal;
	if (bUseNormalMap == 1)
	{
		normal = texture(normalMap, texcoord).rgb;
		normal = normalize(normal*2.0-1.0);
		normal = normalize(tbn * normal);
	}
	
	vec3 N = normalize(normal);

	vec3 emissive = pow3(texture(emissiveMap, texcoord).rgb, 2.2);
	emissive += pow3(emissiveFactor, 2.2);
	emissive *= emissiveIntensity;

	float metallic = metallicFactor + texture(metallicRoughnessMap, texcoord).b;
	float roughness = roughnessFactor + texture(metallicRoughnessMap, texcoord).g;

	aPosition = worldPos;
	aNormal = N;
	aAlbedo = albedo;
	aMetallicRoughness.r = metallic;
	aMetallicRoughness.g = roughness;
	aEmissiveAO.rgb = emissive.rgb;
	aEmissiveAO.a = ao;
}