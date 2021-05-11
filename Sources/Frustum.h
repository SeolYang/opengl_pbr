#pragma once
// https://gist.github.com/podgorskiy/e698d18879588ada9014768e3e82a644
#include "AABB.h"
#include <array>

// Plane : ax+bx+dy+c = 0
class Frustum
{
public:
   void Construct(const glm::mat4& view, const glm::mat4& proj)
   {
      glm::mat4 m = glm::transpose(proj * view);
      m_planes[Left] = m[3] + m[0];
      m_planes[Right] = m[3] - m[0];
      m_planes[Bottom] = m[3] + m[1];
      m_planes[Top] = m[3] - m[1];
      m_planes[Near] = m[3] + m[2];
      m_planes[Far] = m[3] - m[2];

      glm::vec3 crosses[Combinations] = {
         glm::cross(glm::vec3(m_planes[Left]),   glm::vec3(m_planes[Right])),
         glm::cross(glm::vec3(m_planes[Left]),   glm::vec3(m_planes[Bottom])),
         glm::cross(glm::vec3(m_planes[Left]),   glm::vec3(m_planes[Top])),
         glm::cross(glm::vec3(m_planes[Left]),   glm::vec3(m_planes[Near])),
         glm::cross(glm::vec3(m_planes[Left]),   glm::vec3(m_planes[Far])),
         glm::cross(glm::vec3(m_planes[Right]),  glm::vec3(m_planes[Bottom])),
         glm::cross(glm::vec3(m_planes[Right]),  glm::vec3(m_planes[Top])),
         glm::cross(glm::vec3(m_planes[Right]),  glm::vec3(m_planes[Near])),
         glm::cross(glm::vec3(m_planes[Right]),  glm::vec3(m_planes[Far])),
         glm::cross(glm::vec3(m_planes[Bottom]), glm::vec3(m_planes[Top])),
         glm::cross(glm::vec3(m_planes[Bottom]), glm::vec3(m_planes[Near])),
         glm::cross(glm::vec3(m_planes[Bottom]), glm::vec3(m_planes[Far])),
         glm::cross(glm::vec3(m_planes[Top]),    glm::vec3(m_planes[Near])),
         glm::cross(glm::vec3(m_planes[Top]),    glm::vec3(m_planes[Far])),
         glm::cross(glm::vec3(m_planes[Near]),   glm::vec3(m_planes[Far]))
      };

      m_points[0] = Intersection<Left, Bottom, Near>(crosses);
      m_points[1] = Intersection<Left, Top, Near>(crosses);
      m_points[2] = Intersection<Right, Bottom, Near>(crosses);
      m_points[3] = Intersection<Right, Top, Near>(crosses);
      m_points[4] = Intersection<Left, Bottom, Far>(crosses);
      m_points[5] = Intersection<Left, Top, Far>(crosses);
      m_points[6] = Intersection<Right, Bottom, Far>(crosses);
      m_points[7] = Intersection<Right, Top, Far>(crosses);
   }

   bool IsVisible(const AABB& b)
   {
      for (int idx = 0; idx < Count; ++idx)
      {
         if ((glm::dot(m_planes[idx], glm::vec4(b.Min.x, b.Min.y, b.Min.z, 1.0f)) < 0.0f) &&
            (glm::dot(m_planes[idx], glm::vec4(b.Max.x, b.Min.y, b.Min.z, 1.0f)) < 0.0f) &&
            (glm::dot(m_planes[idx], glm::vec4(b.Min.x, b.Max.y, b.Min.z, 1.0f)) < 0.0f) &&
            (glm::dot(m_planes[idx], glm::vec4(b.Max.x, b.Max.y, b.Min.z, 1.0f)) < 0.0f) &&
            (glm::dot(m_planes[idx], glm::vec4(b.Min.x, b.Min.y, b.Max.z, 1.0f)) < 0.0f) &&
            (glm::dot(m_planes[idx], glm::vec4(b.Max.x, b.Min.y, b.Max.z, 1.0f)) < 0.0f) &&
            (glm::dot(m_planes[idx], glm::vec4(b.Min.x, b.Max.y, b.Max.z, 1.0f)) < 0.0f) &&
            (glm::dot(m_planes[idx], glm::vec4(b.Max.x, b.Max.y, b.Max.z, 1.0f)) < 0.0f))
         {
            return false;
         }
      }

      int out = 0;
      out = 0; for (int i = 0; i < 8; i++) out += ((m_points[i].x > b.Max.x) ? 1 : 0); if (out == 8) return false;
      out = 0; for (int i = 0; i < 8; i++) out += ((m_points[i].x < b.Min.x) ? 1 : 0); if (out == 8) return false;
      out = 0; for (int i = 0; i < 8; i++) out += ((m_points[i].y > b.Max.y) ? 1 : 0); if (out == 8) return false;
      out = 0; for (int i = 0; i < 8; i++) out += ((m_points[i].y < b.Min.y) ? 1 : 0); if (out == 8) return false;
      out = 0; for (int i = 0; i < 8; i++) out += ((m_points[i].z > b.Max.z) ? 1 : 0); if (out == 8) return false;
      out = 0; for (int i = 0; i < 8; i++) out += ((m_points[i].z < b.Min.z) ? 1 : 0); if (out == 8) return false;

      return true;
   }

   static void NormalizePlane(glm::vec4& p)
   {
      const float mag = std::sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
      p /= mag;
   }

private:
   enum Planes
   {
      Left = 0,
      Right,
      Bottom,
      Top,
      Near,
      Far,
      Count,
      Combinations = Count * ((Count - 1)) / 2
   };

   template <Planes i, Planes j>
   struct ij2k
   {
      enum { k = i * (9 - i) / 2 + j - 1 };
   };

   template <Planes a, Planes b, Planes c>
   glm::vec3 Intersection(const glm::vec3* crosses) const
   {
      const float d = glm::dot(glm::vec3(m_planes[a]), crosses[ij2k<b, c>::k]);
      const glm::vec3 res = glm::mat3(crosses[ij2k<b, c>::k], -crosses[ij2k<a, c>::k], crosses[ij2k<a, b>::k]) * glm::vec3(m_planes[a].w, m_planes[b].w, m_planes[c].w);
      return res * (-1.0f / d);
   }

private:
   std::array<glm::vec4, 6> m_planes;
   std::array<glm::vec3, 8> m_points;

};