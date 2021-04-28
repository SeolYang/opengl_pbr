#pragma once
#include <glm/glm.hpp>

/*
* @ float3 : POSITION
* @ float2 : TEXCOORD0
* @ float3 : NORMAL
* @ float3 : TANGENT
**/
struct VertexPosTexNT
{
   glm::vec3 Position;
   glm::vec2 TexCoord;
   glm::vec3 Normal;
   glm::vec3 Tangent;
};
