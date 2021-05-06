#pragma once
#include "Scene.h"

class CornellBoxScene : public Scene
{
public:
   CornellBoxScene() = default;
   ~CornellBoxScene() = default;

   void Construct() override;
   void Update(float dt) override;
   void KeyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods) override;

private:
   void UpdateLightRotation();

private:
   Light* m_mainLight = nullptr;
   float m_lightRotationX = 90.0f;
   float m_lightRotationY = 0.0f;
   float m_lightRotationSpeed = 30.0f;
   float m_lightRotationDirX = 0.0f;
   float m_lightRotationDirY = 0.0f;

};