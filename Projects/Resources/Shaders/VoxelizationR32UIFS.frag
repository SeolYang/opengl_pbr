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

uniform int bOverrideBaseColor = 0;
uniform int bOverrideMetallicRoughness = 0;
uniform int bOverrideEmissive = 0;

uniform sampler2DShadow shadowMap;

/* Uniforms */
uniform DirectionalLight light;
uniform vec3 camPos;
layout(r32ui) uniform volatile coherent uimage3D voxelVolume;

/* Predefined Functions */
vec4 ConvertRGBA8ToVec4(uint val)
{
    return vec4(float((val & 0x000000FF)), 
                float((val & 0x0000FF00) >> 8U), 
                float((val & 0x00FF0000) >> 16U), 
                float((val & 0xFF000000) >> 24U));
}

uint ConvertVec4ToRGBA8(vec4 val)
{
    return (uint(val.w) & 0x000000FF) << 24U | 
           (uint(val.z) & 0x000000FF) << 16U | 
           (uint(val.y) & 0x000000FF) << 8U  | 
           (uint(val.x) & 0x000000FF);
}

void ImageAtomicRGBA8Avg(layout(r32ui) volatile coherent uimage3D image, ivec3 coords, vec4 value)
{
    value.rgb *= 255.0;                 // optimize following calculations
    uint newVal = ConvertVec4ToRGBA8(value);
    uint prevStoredVal = 0;
    uint curStoredVal;
	int iter = 0;
	const int maxIterations = 255;

    while((curStoredVal = imageAtomicCompSwap(image, coords, prevStoredVal, newVal)) != prevStoredVal && iter < maxIterations)
    {
        prevStoredVal = curStoredVal;
        vec4 rval = ConvertRGBA8ToVec4(curStoredVal);
        rval.rgb = (rval.rgb * rval.a); // Denormalize
        vec4 curValF = rval + value;    // Add
        curValF.rgb /= curValF.a;       // Renormalize
        newVal = ConvertVec4ToRGBA8(curValF);
		++iter;
    }
}

vec4 LambertianDiffuse()
{
	float visibility = texture(shadowMap, vec3(shadowPosFrag.xy, (shadowPosFrag.z - 0.0005f) / (shadowPosFrag.w + 0.00001f)));
	vec4 albedo = baseColorFactor;
	if (bOverrideBaseColor != 1)
	{
		albedo = texture(baseColorMap, texCoordsFrag).rgba;
		albedo.xyz = pow(albedo.xyz, vec3(2.2));
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
	return vec4(Lo, albedo.a);
}

void main()
{
	vec4 color = LambertianDiffuse();
	if (color.a < 0.1)
	{
		return;
	}

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

	ImageAtomicRGBA8Avg(voxelVolume, voxelPos, color);
}