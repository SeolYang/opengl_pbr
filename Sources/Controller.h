#pragma once
#include "Camera.h"
#include <GLFW/glfw3.h>

constexpr float PI = 3.141592f;
class Controller
{
public:
   Controller(Camera* cam, GLFWwindow* window) :
   m_window(window),
   m_target(cam)
   {
   }

   void Update(float dt)
   {
      if (m_target != nullptr && m_window != nullptr)
      {
         glm::vec3 camPos = m_target->GetPosition();

         double x = 0.0;
         double y = 0.0;
         glfwGetCursorPos(m_window, &x, &y);

         const double dx = m_prevCursorPos.x - x;
         const double dy = m_prevCursorPos.y - y;
         m_horizontalAngle += MouseSensitivity * dt * static_cast<float>(dx);
         m_verticalAngle += MouseSensitivity * dt * static_cast<float>(dy);

         constexpr float epsilon = 0.001f;
         if (m_verticalAngle >= (PI / 2.0f))
         {
            m_verticalAngle = (PI / 2.0f) - epsilon;
         }
         else if (m_verticalAngle <= -(PI / 2.0f))
         {
            m_verticalAngle = -(PI / 2.0f) + epsilon;
         }

         m_forward = glm::vec3(
            std::cos(m_verticalAngle) * std::sin(m_horizontalAngle),
            std::sin(m_verticalAngle),
            std::cos(m_verticalAngle) * std::cos(m_horizontalAngle));

         m_right = glm::vec3(
            std::sin(m_horizontalAngle - PI / 2.0f),
            0.0f,
            std::cos(m_horizontalAngle - PI / 2.0f));

         m_prevCursorPos = glm::vec2(x, y);

         const float deltaSpeed = CameraSpeed * dt * (m_bBoost ? SpeedMultiplier : 1.0f);
         const glm::vec3 deltaForward = m_forward * deltaSpeed;
         const glm::vec3 deltaRight = m_right * deltaSpeed;
         const glm::vec3 deltaUp = glm::vec3(0.0f, 1.0f, 0.0f) * deltaSpeed;
         if (m_bMoveForward)
         {
            camPos += deltaForward;
         }
         if (m_bMoveBackward)
         {
            camPos -= deltaForward;
         }
         if (m_bMoveLeft)
         {
            camPos -= deltaRight;
         }
         if (m_bMoveRight)
         {
            camPos += deltaRight;
         }
         if (m_bMoveUp)
         {
            camPos += deltaUp;
         }
         if (m_bMoveDown)
         {
            camPos -= deltaUp;
         }

         m_target->SetPosition(camPos);
         m_target->SetLookAt(camPos + m_forward);
      }
   }

   void KeyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods)
   {
      const bool bKeyPressed = (action == GLFW_PRESS) || (action == GLFW_REPEAT);
      switch (key)
      {
      default:
         break;
      case GLFW_KEY_W:
         m_bMoveForward = bKeyPressed;
         break;

      case GLFW_KEY_S:
         m_bMoveBackward = bKeyPressed;
         break;

      case GLFW_KEY_A:
         m_bMoveLeft = bKeyPressed;
         break;

      case GLFW_KEY_D:
         m_bMoveRight = bKeyPressed;
         break;

      case GLFW_KEY_SPACE:
         m_bMoveUp = bKeyPressed;
         break;

      case GLFW_KEY_LEFT_CONTROL:
      case GLFW_KEY_RIGHT_CONTROL:
         m_bMoveDown = bKeyPressed;
         break;

      case GLFW_KEY_LEFT_SHIFT:
      case GLFW_KEY_RIGHT_SHIFT:
         m_bBoost = bKeyPressed;
         break;
      }
   }

public:
   float MouseSensitivity = 0.25f;
   float CameraSpeed = 1.0f;
   float SpeedMultiplier = 2.5f;

private:
   GLFWwindow* m_window = nullptr;
   Camera* m_target = nullptr;
   float m_horizontalAngle = PI;
   float m_verticalAngle = 0.0f;
   glm::tvec2<double> m_prevCursorPos = glm::vec2();

   glm::vec3 m_forward;
   glm::vec3 m_right;

   bool m_bMoveForward = false;
   bool m_bMoveBackward = false;
   bool m_bMoveRight = false;
   bool m_bMoveLeft = false;
   bool m_bMoveUp = false;
   bool m_bMoveDown = false;

   bool m_bBoost = false;
   
};
