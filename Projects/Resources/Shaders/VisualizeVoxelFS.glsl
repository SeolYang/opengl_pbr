#version 450 core
#define STEP_LENGTH 0.0006f
#define INV_STEP_LENGTH (1.0f/STEP_LENGTH)

uniform sampler2D cubeBack;
uniform sampler2D cubeFront;
uniform sampler3D voxelVolume;
//uniform usampler3D voxelVolume;
uniform vec3 camPos;
uniform int sampleLevel = 0;

in vec2 texCoordsFrag;
out vec4 color;

vec3 Scale(vec3 v)
{
   return 0.5 * v + vec3(0.5);
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

void main()
{
   const float mipMapLevel = sampleLevel;
   const vec3 origin = IsInsideCube(camPos, 0.2f) ? camPos : texture(cubeFront, texCoordsFrag).xyz;
   vec3 direction = texture(cubeBack, texCoordsFrag).xyz - origin;
   const uint numberOfSteps = uint(length(direction) * INV_STEP_LENGTH);
   direction = normalize(direction);

   color = vec4(0.0f);
   for (uint step = 0; step < numberOfSteps && color.a < 0.99f; ++step)
   {
      const vec3 currentPos = origin + (STEP_LENGTH * step * direction);
      vec3 coordinate = Scale(currentPos);
      //uint currentSampleRGBA8 = textureLod(voxelVolume, coordinate, mipMapLevel).r;
      //vec4 currentSample = convRGBA8ToVec4(currentSampleRGBA8);
      vec4 currentSample = textureLod(voxelVolume, coordinate, mipMapLevel);
      color += (1.0f-color.a) * currentSample;
   }

   color.rgb = color.rgb / (color.rgb + vec3(1.0));
   color.rgb = pow(color.rgb, vec3(1.0 / 2.2));
}