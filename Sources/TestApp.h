#pragma once
#include "Application.h"

class Model;
class Camera;
class TestApp : public Application
{
public:
	TestApp(const std::string& title,
		unsigned int width, unsigned int height) :
		m_helmet(nullptr),
		m_cam(nullptr),
		Application(title, 
			width, height)
	{
	}

	virtual bool Init() override;
	virtual void Update(float dt) override;

private:
	Model* m_helmet;
	Model* m_avocado;
	Model* m_duck;

	Camera* m_cam;

	float m_movePower = 50.0f;
	float m_rotateRad = 6.0f;
	float m_elasedTime = 0.0f;

	float m_rotatePower = 50.0f;
	float m_duckAngle = 0.0f;

};
