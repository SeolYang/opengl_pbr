#version 450 core
in vec3 worldPosFrag;
in vec4 shadowPosFrag;
in vec2 texCoordsFrag;
in vec3 worldNormalFrag;
in mat3 tbnFrag;

out vec4 fragColor;

struct DirectionalLight
{
   vec3 Direction;
   vec3 Intensity;
};

const float PI = 3.14159265359;

/* Material Uniforms */
uniform sampler2D baseColorMap; // baseColorMap: sRGB
uniform vec4 baseColorFactor;
uniform sampler2D normalMap;
uniform sampler2D metallicRoughnessMap; // metallicRoughnessMap: Linear(B:Metallic, G:Roughness)
uniform float metallicFactor;
uniform float roughnessFactor;
uniform sampler2D aoMap; // aoMap(Ambient Occlusion Map): Linear(R channel only)
uniform sampler2D emissiveMap; // emissiveMap: sRGB
uniform vec3 emissiveFactor;
uniform int bUseNormalMap;

/* Uniforms */
uniform DirectionalLight light;
uniform vec3 camPos;
uniform sampler2DShadow shadowMap;
uniform sampler3D voxelVolume;
uniform float voxelGridWorldSize;
uniform float voxelDim;

/* Brdf */
vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float nom = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return nom / max(denom, 0.001);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

/* Voxel Cone Tracing */
mat3 tangentToWorld;
const float MaxDist = 100.0f;
const float AlphaThreshold = 0.95f;
const int NumOfCones = 6;
vec3 coneDirections[6] = vec3[](
	vec3(0.0, 1.0, 0.0),
	vec3(0.0, 0.5, 0.866025),
	vec3(0.823639, 0.5, 0.267617),
	vec3(0.509037, 0.5, -0.700629),
	vec3(-0.509037, 0.5, -0.700629),
	vec3(-0.823639, 0.5, 0.267617));

float coneWeights[6] = float[](0.25f, 0.15f, 0.15f, 0.15f, 0.15f, 0.15f);

vec4 SampleVoxelVolume(vec3 worldPos, float lod)
{
	vec3 offset = vec3(1.0f/voxelDim, 1.0f/voxelDim,  0.0f);
	vec3 voxelVolumeUV = worldPos / (voxelGridWorldSize*0.5f);
	voxelVolumeUV = voxelVolumeUV * 0.5 + 0.5 + offset;
	return textureLod(voxelVolume, voxelVolumeUV, lod);
}

vec4 ConeTrace(vec3 normal, vec3 direction, float tanHalfAngle, out float occlusion)
{
	float lod = 0.0f;
	vec3 color = vec3(0.0f);
	float alpha = 0.0f;
	occlusion = 0.0f;

	float voxelSize = voxelGridWorldSize / voxelDim;
	float dist = voxelSize;
	vec3 origin = worldPosFrag + (worldNormalFrag*voxelSize);

	while (dist < MaxDist && alpha < AlphaThreshold)
	{
		float coneDiameter = max(voxelSize, 2.0f*tanHalfAngle*dist);
		float lodLevel = log2(coneDiameter / voxelSize);
		vec4 voxelColor = SampleVoxelVolume(origin+(dist*direction), lodLevel);

		float a = (1.0 - alpha);
		color += a*voxelColor.rgb;
		alpha += a*voxelColor.a;
		occlusion += (a*voxelColor.a)/(1.0 + (0.03*coneDiameter));
		dist += coneDiameter*0.5;
	}

	return vec4(color, alpha);
}

vec4 IndirectLight(vec3 normal, out float occlusionOut)
{
	vec4 color = vec4(0.0f);
	occlusionOut = 0.0f;
	for (int cone = 0; cone < NumOfCones; ++cone)
	{
		// tan(pi/6) = 0.577 (pi/6 rad = 30 degrees)
		float occlusion = 0.0f;
		color += coneWeights[cone] *
		 ConeTrace(normal, tangentToWorld*coneDirections[cone], 0.577, occlusion);
		occlusionOut += coneWeights[cone] * occlusion;
	}

	occlusionOut = 1.0-occlusionOut;
	return color;
}

void main()
{
	vec4 albedo = texture(baseColorMap, texCoordsFrag).rgba;
	if (albedo.a < 0.5)
	{
		discard;
	}
	albedo = vec4(pow(albedo.rgb, vec3(2.2)) + baseColorFactor.rgb, albedo.a);

	tangentToWorld = inverse(tbnFrag);
	vec3 emissive = pow(texture(emissiveMap, texCoordsFrag).rgb, vec3(2.2)) + emissiveFactor;

	float ao = texture(aoMap, texCoordsFrag).r;

	float metallic = metallicFactor + texture(metallicRoughnessMap, texCoordsFrag).b;
	float roughness = roughnessFactor + texture(metallicRoughnessMap, texCoordsFrag).g;

	vec3 normal = worldNormalFrag;
	if (bUseNormalMap == 1)
	{
		normal = texture(normalMap, texCoordsFrag).rgb;
		normal = normalize(normal * 2.0 - 1.0);
		normal = normalize(tbnFrag * normal);
	}

	vec3 N = normalize(normal);
	vec3 V = normalize(camPos - worldPosFrag);
	vec3 L = normalize(-light.Direction);
	vec3 H = normalize(V + L);
	float NdotL = max(dot(N, L), 0.0);

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo.rgb, metallic);
	vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);
	vec3 kS = F;
	vec3 kD = vec3(1.0)-kS;
	kD *= (1.0-metallic);

	vec3 diffuseReflection;
	float visibility = texture(shadowMap, vec3(shadowPosFrag.xy, (shadowPosFrag.z - 0.0005f)/(shadowPosFrag.w + 0.0001f)));

	/* Direct Diffuse ( Lambertian Diffuse ) */
	vec3 directDiffuseLight = vec3(visibility * NdotL);

	float occlusion = 0.0f;
	vec3 indirectDiffuseLight = 4.0*IndirectLight(N, occlusion).rgb;

	occlusion = min(1.0, 1.5*occlusion);
	diffuseReflection = 2.0 * occlusion * (albedo).rgb * (directDiffuseLight+indirectDiffuseLight);

	fragColor = vec4(diffuseReflection, albedo.a);
	fragColor.xyz = fragColor.xyz/(fragColor.xyz+vec3(1.0));
	fragColor.xyz = pow(fragColor.xyz, vec3(1.0/2.2));
}