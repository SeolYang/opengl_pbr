#pragma once
#include "Scene.h"
#include "CameraPath.h"

class SponzaScene : public Scene
{
public:
	SponzaScene() = default;
   ~SponzaScene() = default;

	void Construct() override;
	void Update(float dt) override;
	void KeyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods) override;

private:
	void UpdateLightRotation();

private:
	Model* m_sponza = nullptr;
	Model* m_sphere = nullptr;
	Model* m_quad = nullptr;

	Light* m_mainLight = nullptr;

	Camera* m_cam;

	float m_elapsedTime = 0.0f;
	float m_lightRotationX = 100.0f;
	float m_lightRotationY = -20.0f;
	float m_lightRotationSpeed = 30.0f;
	float m_lightRotationDirX = 0.0f;
	float m_lightRotationDirY = 0.0f;

	float m_sphereOrbitRad = 10.0f;

	Material* m_floorMaterial = nullptr;
	float m_floorRoughness = 1.0f;

	CameraPath m_camPath;
	bool m_bEnableCamPath = false;

	bool m_bLightRotate = false;
	float m_lightElapsedTime = 0.0f;
	float m_lightRotateSpeed = 2.0f;

};
