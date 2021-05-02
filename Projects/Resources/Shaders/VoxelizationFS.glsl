#version 450 core
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
uniform int bUseNormalMap;
uniform sampler2D metallicRoughnessMap; // metallicRoughnessMap: Linear(B:Metallic, G:Roughness)
uniform float metallicFactor;
uniform float roughnessFactor;
uniform sampler2D aoMap; // aoMap(Ambient Occlusion Map): Linear(R channel only)
uniform sampler2D emissiveMap; // emissiveMap: sRGB
uniform vec3 emissiveFactor;
uniform float emissiveIntensity;

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

vec4 LambertianDiffuse()
{
	float visibility = (texture(shadowMap, vec3(shadowPosFrag.xy, (shadowPosFrag.z - 0.0005f) / (shadowPosFrag.w + 0.0001f)))) > 0.0 ? 1.0 : 0.0;
	vec4 albedo = texture(baseColorMap, texCoordsFrag).rgba;
	albedo = vec4((pow3(albedo.rgb, 2.2) + baseColorFactor.rgb), albedo.a);

	vec3 emissive = emissiveIntensity * (pow3(texture(emissiveMap, texCoordsFrag).rgb, 2.2) + emissiveFactor);

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
	vec3 Lo = emissive + ((albedo.rgb / PI) * light.Intensity * NdotL * visibility);
	return vec4(Lo, 1.0f);
}

vec4 Color()
{
	float visibility = texture(shadowMap, vec3(shadowPosFrag.xy, (shadowPosFrag.z - 0.0005f) / (shadowPosFrag.w + 0.00001f)));
	vec4 albedo = texture(baseColorMap, texCoordsFrag).rgba;
	albedo = vec4((pow3(albedo.rgb, 2.2) + baseColorFactor.rgb), albedo.a);

	vec3 emissive = emissiveIntensity * (pow3(texture(emissiveMap, texCoordsFrag).rgb, 2.2) + emissiveFactor);

	vec3 Lo = emissive + (albedo.rgb * visibility);
	return vec4(Lo, 1.0f);
}

void main()
{
	vec4 color = LambertianDiffuse();
	vec3 voxel = ScaleAndBias(worldPosFrag);
	ivec3 dimension = imageSize(voxelVolume);
	ivec3 voxelCamPos = ivec3(gl_FragCoord.x, gl_FragCoord.y, dimension.x * gl_FragCoord.z);
	ivec3 voxelPos;
	if (axisFrag == 0)
	{
		voxelPos.x = dimension.x - voxelCamPos.z;
		voxelPos.z = voxelCamPos.x;
		voxelPos.y = voxelCamPos.y;
	}
	else if (axisFrag == 1)
	{
		voxelPos.z = voxelCamPos.y;
		voxelPos.y = dimension.x - voxelCamPos.z;
		voxelPos.x = voxelCamPos.x;
	}
	else
	{
		voxelPos = voxelCamPos;
	}

	voxelPos.z = dimension.x - voxelPos.z - 1;

	imageStore(voxelVolume, voxelPos, color);
	//imageAtomicRGBA8Avg(voxelVolume, ivec3(dimension * voxel), color);
}