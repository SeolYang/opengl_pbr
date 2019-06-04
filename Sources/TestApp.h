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
		m_secondaryLight(nullptr),
		m_cam(nullptr),
		m_duckMat(nullptr),
		Application(title, 
			width, height)
	{
	}

protected:
	virtual bool Init() override;
	virtual void Update(float dt) override;

	virtual void WindowResizeCallback(GLFWwindow* window,
		int width, int height) override;

private:
	Model*		m_helmet;
	Model*		m_avocado;
	Model*		m_duck;
	Model*		m_spheres;
				
	Light*		m_mainLight;
	Light*		m_secondaryLight;
				
	Camera*		m_cam;
				
	float		m_movePower = 50.0f;
	float		m_rotateRad = 5.0f;
	float		m_elasedTime = 0.0f;
				
	float		m_rotatePower = 50.0f;
	float		m_duckAngle = 0.0f;
				
	float		m_duckRoughness = 0.0f;
	float		m_duckRoughnessScale = 0.03f;
	Material*	m_duckMat = nullptr;

};
