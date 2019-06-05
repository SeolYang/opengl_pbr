#pragma once
#include "Application.h"

class Model;
class Camera;
class Material;
class Light;
class TestApp : public Application
{
public:
	TestApp(const std::string& title,
		unsigned int width, unsigned int height) :
		m_helmet(nullptr),
		m_avocado(nullptr),
		m_duck(nullptr),
		m_spheres(nullptr),
		m_mainLight(nullptr),
		m_cam(nullptr),
		m_secondaryCam(nullptr),
		m_duckMat(nullptr),
		m_madoka(nullptr),
		m_mikuStand(nullptr),
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
	Model*		m_helmet;
	Model*		m_avocado;
	Model*		m_duck;
	Model*		m_spheres;
	Model*		m_madoka;
	Model*		m_mikuStand;
				
	Light*		m_mainLight;
				
	Camera*		m_cam;
	Camera*		m_secondaryCam;
				
	float		m_movePower = 50.0f;

	float		m_elasedTime = 0.0f;
	
	bool		m_bRotateCam = true;
	float		m_rotateRad = 5.0f;
	float		m_rotateRadDiff = 0.5f;

	float		m_camYDiff = 0.5f;
	float		m_camY = 0.0f;

	float		m_duckRotatePower = 50.0f;
	float		m_powerDiff = 10.0f;
	float		m_duckAngle = 0.0f;
	float		m_duckRoughness = 0.0f;
	float		m_duckRoughnessScale = 0.03f;
	Material*	m_duckMat = nullptr;

};
