#pragma once
#include "Application.h"
#include "CameraPath.h"
#include <glm/glm.hpp>

class Model;
class Camera;
class Controller;
class Material;
class Light;
class TestApp : public Application
{
public:
	TestApp(const std::string& title,
		unsigned int width, unsigned int height, bool bFullScreen) :
		Application(title, width, height, bFullScreen)
	{
	}

protected:
	virtual bool Init() override;
	virtual void Update(float dt) override;

	virtual void WindowResizeCallback(GLFWwindow* window,
		int width, int height) override;

	virtual void KeyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods);

private:
	void UpdateLightRotation();

private:
	Model* m_sponza = nullptr;
	Model* m_sphere = nullptr;
	Model* m_quad = nullptr;
				
	Light* m_mainLight = nullptr;
				
	Camera*		m_cam;
	Controller* m_controller = nullptr;
				
	float m_elapsedTime = 0.0f;
	float m_lightRotationX = 100.0f;
	float m_lightRotationY = -20.0f;

	float m_sphereOrbitRad = 10.0f;

	Material* m_floorMaterial = nullptr;
	float m_floorRoughness = 0.0f;

	CameraPath m_camPath;
	bool m_bEnableCamPath = false;

};
