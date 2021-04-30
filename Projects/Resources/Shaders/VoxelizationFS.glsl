#version 450 core

const unsigned int MaximumLights = 128;
const float PI = 3.14159265359;

struct DirectionalLight
{
   vec3 Direction;
   vec3 Intensity;
};

/* Input from previous shader stage */
in vec3 worldPosFrag;
in vec4 shadowPosFrag;
in vec2 texCoordsFrag;
in vec3 worldNormalFrag;
in mat3 tbnFrag;
in mat4 projFrag;
in flat int axisFrag;

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

uniform sampler2DShadow shadowMap;

/* Uniforms */
//uniform Light lights[MaximumLights];
uniform DirectionalLight light;
uniform vec3 camPos;
layout(RGBA8) uniform image3D voxelVolume;
//layout(r32ui) coherent volatile uniform uimage3D voxelVolume;

/* Predefined Functions */
vec2 pow2(vec2 v, float power)
{
   return vec2(pow(v.x, power), pow(v.y, power));
}

vec3 pow3(vec3 v, float power)
{
   return vec3(pow(v.x, power), pow(v.y, power), pow(v.z, power));
}

vec4 pow4(vec4 v, float power)
{
   return vec4(pow(v.x, power), pow(v.y, power), pow(v.z, power), pow(v.w, power));
}

vec3 ScaleAndBias(vec3 v)
{
	return 0.5f * v + vec3(0.5f);
}

bool IsInsideCube(vec3 v, float e)
{
	return (abs(v.x) < (1.0 + e)) && (abs(v.y) < (1.0 + e)) && (abs(v.z) < (1.0 + e));
}

vec4 convRGBA8ToVec4(uint val)
{
	return vec4(
		float((val & 0x000000FF)),
		float((val & 0x0000FF00) >> 8U),
		float((val & 0x00FF0000) >> 16U), 
		float((val & 0xFF000000) >> 24U));
}

uint convVec4ToRGBA8(vec4 val)
{
	return (uint(val.w) & 0x000000FF) << 24U |
		(uint(val.z) & 0x000000FF) << 16U |
		(uint(val.y) & 0x000000FF) << 8U |
		(uint(val.x) & 0x000000FF);
}

void imageAtomicRGBA8Avg(layout(r32ui) coherent volatile uimage3D img, ivec3 coords, vec4 val)
{
	val.rgb *= 255.0f;
	uint newVal = convVec4ToRGBA8(val);
	uint prevStoredVal = 0;
	uint curStoredVal;

	while ((curStoredVal = imageAtomicCompSwap(img, coords, prevStoredVal, newVal)) != prevStoredVal)
	{
		prevStoredVal = curStoredVal;
		vec4 rval = convRGBA8ToVec4(curStoredVal);
		rval.xyz = (rval.xyz * rval.w);
		vec4 curValF = rval + val;
		curValF.xyz /= (curValF.w);
		newVal = convVec4ToRGBA8(curValF);
	}
}

/* BRDF */
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

vec4 LambertianDiffuse()
{
	float visibility = texture(shadowMap, vec3(shadowPosFrag.xy, (shadowPosFrag.z - 0.0005f) / (shadowPosFrag.w + 0.00001f)));
	if (visibility > 0.0)
	{
		vec4 albedo = texture(baseColorMap, texCoordsFrag).rgba;
		albedo = vec4((pow3(albedo.rgb, 2.2) + baseColorFactor.rgb), albedo.a);
		
		float metallic = min(1.0, metallicFactor + texture(metallicRoughnessMap, texCoordsFrag).b);

		vec3 emissive = pow3(texture(emissiveMap, texCoordsFrag).rgb, 2.2) + emissiveFactor;

		float ao = texture(aoMap, texCoordsFrag).r;
		vec3 ambient = vec3(0.03) * albedo.rgb * ao;

		vec3 normal = worldNormalFrag;
		if (bUseNormalMap == 1)
		{
			normal = texture(normalMap, texCoordsFrag).rgb;
			normal = normalize(normal * 2.0 - 1.0);
			normal = normalize(tbnFrag * normal);
		}

		vec3 N = normalize(normal);
		vec3 L = -normalize(light.Direction);
		float NdotL = max(dot(N, L), 0.0f);
		vec3 Lo = ambient + emissive + ((albedo.rgb / PI)) * (visibility * light.Intensity * NdotL);
		return vec4(Lo, 1.0f);
	}

	return vec4(0.0f);
}

void main()
{
	vec4 color = LambertianDiffuse();
	vec3 voxel = ScaleAndBias(worldPosFrag);
	ivec3 dimension = imageSize(voxelVolume);
	ivec3 camPos = ivec3(gl_FragCoord.x, gl_FragCoord.y, dimension.x * gl_FragCoord.z);
	ivec3 voxelPos;
	if (axisFrag == 0)
	{
		voxelPos.x = dimension.x - camPos.z;
		voxelPos.z = camPos.x;
		voxelPos.y = camPos.y;
	}
	else if (axisFrag == 1)
	{
		voxelPos.z = camPos.y;
		voxelPos.y = dimension.x - camPos.z;
		voxelPos.x = camPos.x;
	}
	else
	{
		voxelPos = camPos;
	}

	voxelPos.z = dimension.x - voxelPos.z - 1;

	imageStore(voxelVolume, voxelPos, color);
	//imageAtomicRGBA8Avg(voxelVolume, ivec3(dimension * voxel), color);
}