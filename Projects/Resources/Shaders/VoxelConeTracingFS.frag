#version 450 core
in vec3 worldPosFrag;
in vec4 shadowPosFrag;
in vec2 texCoordsFrag;
in vec3 worldNormalFrag;
in mat3 tbnFrag;
in mat3 tnbFrag;

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
uniform int bUseNormalMap;
uniform sampler2D metallicRoughnessMap; // metallicRoughnessMap: Linear(B:Metallic, G:Roughness)
uniform float metallicFactor;
uniform float roughnessFactor;
uniform sampler2D aoMap; // aoMap(Ambient Occlusion Map): Linear(R channel only)
uniform sampler2D emissiveMap; // emissiveMap: sRGB
uniform vec3 emissiveFactor;
uniform float emissiveIntensity;
uniform float ior = 1.0;
uniform int isRefract = 0;

uniform int bOverrideBaseColor = 0;
uniform int bOverrideMetallicRoughness = 0;
uniform int bOverrideEmissive = 0;

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

vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
	return F0 + (max(vec3(1.0-roughness), F0) - F0) * pow(max(1.0-cosTheta, 0.0), 5.0);
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

float GeometrySchlickGGXIndirect(float NdotV, float roughness)
{
	float a = roughness;
	float k = (a*a) / 2.0;

	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}

float GeometrySmithIndirect(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGXIndirect(NdotV, roughness);
	float ggx1 = GeometrySchlickGGXIndirect(NdotL, roughness);

	return ggx1 * ggx2;
}

float RadicalInverse_VanDerCorpus(uint bits)
{
   bits = (bits << 16u) | (bits >> 16u);
   bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
   bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
   bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
   bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
   return float(bits) * 2.3283064365386963e-10;
}

vec2 Hammersley(uint idx, uint N)
{
   return vec2(
      (float(idx) / float(N)),
      RadicalInverse_VanDerCorpus(idx));
}

vec3 ImportanceSampleGGX(vec2 Xi, float roughness, vec3 N)
{
	float a = roughness*roughness;
	float phi = 2.0f * PI * Xi.x;
	float cosTheta = sqrt((1.0f-Xi.y)/(1.0f+(a*a-1.0f)*Xi.y));
	float sinTheta = sqrt(1.0f - (cosTheta*cosTheta));

	vec3 H;
	H.x = cos(phi)*sinTheta;
	H.y = sin(phi)*sinTheta;
	H.z = cosTheta;

	vec3 up = abs(N.z) < 0.999f ? vec3(0.0f, 0.0f, 1.0f) : vec3(1.0f, 0.0f, 0.0f);
	vec3 tangent = normalize(cross(up, N));
	vec3 bitangent = cross(N, tangent);

	vec3 sampleVec = tangent*H.x+bitangent*H.y+N*H.z;
	return normalize(sampleVec);
}

/* Voxel Cone Tracing(VCT Params) */
uniform float maxDist_VCT = 300.0f;
uniform float step_VCT = 0.5f;
uniform float alphaThreshold_VCT = 1.0f;
uniform int specularSampleNum_VCT = 4;
uniform float attenuationFactor_VCT = 0.1f;
uniform float initialStep_VCT = 6.5f;
uniform float indirectDiffusePower_VCT = 4.0f;
uniform float indirectSpecularPower_VCT = 2.0f;

uniform int enableDirectDiffuse = 1;
uniform int enableIndirectDiffuse = 1;
uniform int enableDirectSpecular = 1;
uniform int enableIndirectSpecular = 1;
uniform int debugAmbientOcclusion = 0;

mat3 tangentToWorld;
const int NumOfCones = 6;
vec3 coneDirections[6] = vec3[](
	vec3(0.0, 1.0, 0.0),
	vec3(0.0, 0.5, 0.866025),
	vec3(0.823639, 0.5, 0.267617),
	vec3(0.509037, 0.5, -0.700629),
	vec3(-0.509037, 0.5, -0.700629),
	vec3(-0.823639, 0.5, 0.267617));

float coneWeights[6] = float[](0.25f, 0.15f, 0.15f, 0.15f, 0.15f, 0.15f);
//const float coneWeights[6] = float[](5.0/20.0f, 3.0/20.0, 3.0/20.0, 3.0/20.0, 3.0/20.0, 3.0/20.0);
//const float coneWeights[6] = float[](1.0/6.0f, 1.0/6.0f, 1.0/6.0f, 1.0/6.0f, 1.0/6.0f, 1.0/6.0f);

//const int NumOfCones = 16;
//const vec3 coneDirections[16] = {
//    vec3(0.57735, 0.57735, 0.57735),
//    vec3(0.57735, -0.57735, -0.57735),
//    vec3(-0.57735, 0.57735, -0.57735),
//    vec3(-0.57735, -0.57735, 0.57735),
//    vec3(-0.903007, -0.182696, -0.388844),
//    vec3(-0.903007, 0.182696, 0.388844),
//    vec3(0.903007, -0.182696, 0.388844),
//    vec3(0.903007, 0.182696, -0.388844),
//    vec3(-0.388844, -0.903007, -0.182696),
//    vec3(0.388844, -0.903007, 0.182696),
//    vec3(0.388844, 0.903007, -0.182696),
//    vec3(-0.388844, 0.903007, 0.182696),
//    vec3(-0.182696, -0.388844, -0.903007),
//    vec3(0.182696, 0.388844, -0.903007),
//    vec3(-0.182696, 0.388844, 0.903007),
//    vec3(0.182696, -0.388844, 0.903007)
//};
//
//const float coneWeights = (1.0f/16.0f);
//
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
	float dist = initialStep_VCT*voxelSize;
	vec3 origin = worldPosFrag + (dist*normal);

	float attenuation = 1.0f;
	// @TODO Distance base Attenuation
	while (dist < maxDist_VCT && alpha < alphaThreshold_VCT)
	{
		attenuation = min(1.0/(attenuationFactor_VCT*dist), 1.0);
		//attenuation = 1.0/(dist*dist);
		float coneDiameter = max(voxelSize, 2.0f*tanHalfAngle*dist);
		float lodLevel = log2(coneDiameter / voxelSize);
		vec4 voxelColor = SampleVoxelVolume(origin+(dist*direction), lodLevel);

		float a = (1.0 - alpha);
		color += a*voxelColor.rgb*attenuation;
		alpha += a*voxelColor.a;
		occlusion += (a*voxelColor.a)/(1.0 + (0.03*coneDiameter));

		dist += coneDiameter*step_VCT; 
	}

	return vec4(color, alpha);
}

vec4 ConeTraceRefraction(vec3 normal, vec3 direction, float tanHalfAngle, out float occlusion)
{
    vec3 N = -normal;
	float lod = 0.0f;
	vec3 color = vec3(0.0f);
	float alpha = 0.0f;
	occlusion = 0.0f;

	float voxelSize = voxelGridWorldSize / voxelDim;
	float dist = voxelSize;
	vec3 origin = worldPosFrag + (dist*N);

	// @TODO Distance base Attenuation
	while (dist < maxDist_VCT && alpha < alphaThreshold_VCT)
	{
		float coneDiameter = max(voxelSize, 2.0f*tanHalfAngle*dist);
		float lodLevel = log2(coneDiameter / voxelSize);
		vec4 voxelColor = SampleVoxelVolume(origin+(dist*direction), lodLevel);

		float a = (1.0 - alpha);
		color += a*voxelColor.rgb;
		alpha += a*voxelColor.a;
		occlusion += (a*voxelColor.a)/(1.0 + (0.03*coneDiameter));

		dist += coneDiameter*step_VCT; 
	}

	return vec4(color, alpha);
}

vec4 IndirectDiffuse(vec3 normal, out float occlusionOut)
{
	vec4 radiance = vec4(0.0f);
	occlusionOut = 0.0f;
	for (int cone = 0; cone < NumOfCones; ++cone)
	{
		// tan(pi/6) = 0.577 (pi/6 rad = 30 degrees)
		// tan(pi/16) = 0.198912 ~ 0.2
		float occlusion = 0.0f;
		vec3 L = normalize(tangentToWorld*coneDirections[cone]);
		//vec4 tracedRadiance = ConeTrace(normal, L, 0.2, occlusion)*max(dot(normal, L), 0.0f);
		vec4 tracedRadiance = ConeTrace(normal, L, 0.577, occlusion);
		radiance += tracedRadiance*coneWeights[cone];
		occlusionOut += occlusion*coneWeights[cone];
	}

	occlusionOut = 1.0-occlusionOut;
	return radiance;
}

vec3 IndirectSpecular(const uint numSamples, float roughness, vec3 N, vec3 V, vec3 specularColor)
{
	vec3 specularLighting = vec3(0.0f);
	for( uint idx = 0; idx < numSamples; ++idx)
	{
		vec2 xi = Hammersley(idx, numSamples);
		vec3 H = ImportanceSampleGGX(xi, roughness, N);
		vec3 L = normalize(2.0*dot(V, H)*H-V);

		float NoV = clamp(dot(N,V), 0.0, 1.0);
		float NoL = clamp(dot(N,L), 0.0, 1.0);
		float NoH = clamp(dot(N,H), 0.0, 1.0);
		float VoH = clamp(dot(V,H), 0.0, 1.0);

		if (NoL > 0.0)
		{
			float specularOcc = 0.0;
			vec3 tracedColor = ConeTrace(N, L, mix(0.03, 0.75, roughness*roughness), specularOcc).rgb;

			float G = GeometrySmithIndirect(N, V, L, roughness);
			float Fc = pow(1.0-VoH, 5);
			vec3 F = (1.0-Fc)*specularColor+Fc;
			specularLighting += tracedColor*F*G*VoH/(NoH*NoV);
		}
	}
	return specularLighting/float(numSamples);
}

vec3 IndirectSpecularDir(const uint numSamples, float roughness, vec3 N, vec3 V)
{
	vec3 direction = vec3(0.0f);
	for( uint idx = 0; idx < numSamples; ++idx)
	{
		vec2 xi = Hammersley(idx, numSamples);
		vec3 H = ImportanceSampleGGX(xi, roughness, N);
		vec3 L = normalize(2.0*dot(V, H)*H-V);

		direction += L;
	}

	return direction/float(numSamples);
}

void main()
{
	float visibility = texture(shadowMap, vec3(shadowPosFrag.xy, (shadowPosFrag.z - 0.0005f) / (shadowPosFrag.w)));
	vec4 albedo = baseColorFactor;
	if (bOverrideBaseColor != 1)
	{
		albedo = texture(baseColorMap, texCoordsFrag).rgba;
		albedo = vec4(pow(albedo.rgb, vec3(2.2)), albedo.a);
	}

	vec3 emissive = emissiveFactor;
	if (bOverrideEmissive != 1)
	{
		vec4 emissiveColor = texture(emissiveMap, texCoordsFrag).rgba;
		emissive = pow(emissiveColor.rgb, vec3(2.2));
		if (emissiveColor.a < 1.0)
		{
			albedo.a = emissiveColor.a;
		}
	}
	emissive *= emissiveIntensity;

	if (isRefract != 1 && albedo.a < 0.1)
	{
		discard;
	}

	tangentToWorld = tnbFrag;

	float ao = texture(aoMap, texCoordsFrag).r;

	float metallic = metallicFactor;
	float roughness = roughnessFactor;
	if (bOverrideMetallicRoughness != 1)
	{
		metallic = texture(metallicRoughnessMap, texCoordsFrag).b;
		roughness = texture(metallicRoughnessMap, texCoordsFrag).g;
	}

	vec3 normal = normalize(worldNormalFrag);
	if (bUseNormalMap == 1)
	{
		normal = texture(normalMap, texCoordsFrag).rgb;
		normal = normalize(normal * 2.0 - 1.0);
		normal = normalize(tbnFrag * normal);
	}

	vec3 N = normal;
	vec3 V = normalize(camPos - worldPosFrag);
	vec3 L = normalize(-light.Direction);
	vec3 H = normalize(V + L);
	float NdotL = max(dot(N, L), 0.0);

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo.rgb, metallic);

	/* Direct Specular */
	float NDF = DistributionGGX(N, H, roughness);
	float G = GeometrySmith(N, V, L, roughness);
	vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);
	vec3 nominator = NDF*G*F;
	float denominator = 4.0 * max(dot(N,V), 0.0) * max(dot(N, L), 0.0);
	vec3 directSpecular = nominator / max(denominator, 0.001);
	
	vec3 kS_direct = F;
	vec3 kD_direct = vec3(1.0)-kS_direct;
	kD_direct *= (1.0-metallic);

	/* Direct Diffuse ( Lambertian Diffuse ) */
	vec3 directDiffuse = kD_direct * (albedo.rgb/PI);
	
	/* Indirect Specular */
	//vec3 F_indirect = F_reflect;
	//vec3 F_indirect = (FresnelSchlickRoughness(max(dot(N, V), 0.0f), F0, roughness)+F_reflect)/2.0;
	vec3 F_indirect = FresnelSchlickRoughness(max(dot(N, V), 0.0f), F0, roughness);
	vec3 indirectSpecular = indirectSpecularPower_VCT * IndirectSpecular(specularSampleNum_VCT, roughness, N, V, F_indirect);
	vec3 kS_indirect = F_indirect;
	vec3 kD_indirect = vec3(1.0) - kS_indirect;
	kD_indirect *= (1.0-metallic);

	/* Indirect Diffuse */
	float occlusion = 0.0f;
	vec3 indirectDiffuse = indirectDiffusePower_VCT * IndirectDiffuse(N, occlusion).rgb;
	occlusion = 2.0f * min(1.0, 1.5 * occlusion);
	indirectDiffuse =  occlusion * (kD_indirect * indirectDiffuse * (albedo.rgb/PI));

	directDiffuse = (enableDirectDiffuse == 1) ? directDiffuse : vec3(0.0f);
	indirectDiffuse = (enableIndirectDiffuse == 1) ? indirectDiffuse : vec3(0.0f);
	directSpecular = (enableDirectSpecular == 1) ? directSpecular : vec3(0.0f);
	indirectSpecular = (enableIndirectSpecular == 1) ? indirectSpecular : vec3(0.0f);

	vec3 directLight = (directDiffuse + directSpecular) * light.Intensity * NdotL * visibility;
	vec3 indirectLight = (indirectDiffuse+indirectSpecular);

	// Refraction test
	vec3 refracted = vec3(0.0f);
	if (isRefract == 1)
	{
		float socc;
		refracted = ConeTraceRefraction(N, refract(-V, N, 1.0f/ior), 0.03, socc).rgb; // Snell's law n = n_0(air/vaccum ior)/n2
		fragColor = vec4(refracted, 1.0f);
	}
	else
	{
		fragColor = (debugAmbientOcclusion == 1) ? vec4(vec3(occlusion), 1.0f) : vec4(emissive + directLight + indirectLight, albedo.a);
	}

	fragColor.xyz = fragColor.xyz/(fragColor.xyz+vec3(1.0));
	fragColor.xyz = pow(fragColor.xyz, vec3(1.0/2.2));
}