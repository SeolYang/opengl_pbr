#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/spline.hpp>

struct Path
{
public:
   glm::vec3 p0 = glm::vec3(0.0f);
   glm::vec3 p1 = glm::vec3(0.0f);
   glm::quat r0 = glm::quat();
   glm::quat r1 = glm::quat();
   float Speed = 1.0f;

};

// Catmull rom spline / slerp based camera position and rotation interpolation
class CameraPath
{
public:
   glm::vec3 GetCurrentPosition() const
   {
      if (!Paths.empty())
      {
         glm::vec3 p0;
         glm::vec3 p1 = Paths[m_idx].p0;
         glm::vec3 p2 = Paths[m_idx].p1;
         glm::vec3 p3;

         if (m_idx == 0)
         {
            p0 = -p1;
            if ((m_idx + 1) == Paths.size())
            {
               p3 = Paths[m_idx + 1].p0;
            }
            else
            {
               p3 = p2 * 2.0f;
            }
         }
         else if ((m_idx + 1) == Paths.size())
         {
            p0 = Paths[m_idx].p1;
            p3 = p2 * 2.0f;
         }
         else if (m_idx >= Paths.size())
         {
            return Paths[m_idx - 1].p1;
         }
         else
         {
            p0 = Paths[m_idx].p1;
            p3 = Paths[m_idx + 1].p0;
         }

         return glm::catmullRom(p0, p1, p2, p3, m_t);
      }

      return glm::vec3(0.0f);
   }

   glm::quat GetCurrentRotation() const
   {
      if (!Paths.empty())
      {
         if (m_idx < Paths.size())
         {
            return glm::slerp(Paths[m_idx].r0, Paths[m_idx].r1, m_t);
         }

         return glm::slerp(Paths[m_idx-1].r0, Paths[m_idx-1].r1, 0.999f);
      }

      return glm::quat();
   }

   void Update(float dt)
   {
      if (!Paths.empty())
      {
         if (bResetOnFinish && (m_idx >= Paths.size()))
         {
            ResetProgress();
         }
         else if (m_idx < Paths.size())
         {
            m_t += (dt * Paths[m_idx].Speed);
            if (m_t >= 1.0f)
            {
               ++m_idx;
               m_t = 0.0f;
            }
         }
      }
   }

   void ResetProgress()
   {
      m_t = 0.0f;
      m_idx = 0;
   }

public:
   std::vector<Path> Paths;
   bool bResetOnFinish = false;

private:
   float m_t = 0.0f;
   unsigned int m_idx = 0;

};