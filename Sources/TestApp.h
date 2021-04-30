#pragma once
#include "Application.h"

class Model;
class Camera;
class Controller;
class Material;
class Light;
class TestApp : public Application
{
public:
	TestApp(const std::string& title,
		unsigned int width, unsigned int height) :
	   m_sponza(nullptr),
		m_mainLight(nullptr),
		m_cam(nullptr),
		m_secondaryCam(nullptr),
		Application(title, 
			width, height)
	{
	}

protected:
	virtual bool Init() override;
	virtual void Update(float dt) override;

	virtual void WindowResizeCallback(GLFWwindow* window,
		int width, int height) override;

	virtual void KeyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods);

	void RandomLightGen();
	void SplitViewport();

private:
	void UpdateLightRotation();

private:
	Model* m_sponza;
	Model* m_sphere;
				
	Light* m_mainLight;
				
	Camera*		m_cam;
	Camera*		m_secondaryCam;
	Controller* m_controller = nullptr;
				
	float m_elapsedTime = 0.0f;
	float m_lightRotationX = 75.0f;
	float m_lightRotationY = 0.0f;

	float m_sphereOrbitRad = 10.0f;

};
