#include "TestApp.h"
#include "Scene.h"
#include "Model.h"
#include "Camera.h"
#include "Material.h"
#include "Light.h"
#include "Viewport.h"
#include "Renderer.h"

#include "GLFW/glfw3.h"

#include <iostream>
#include <cmath>
#include <random>

bool TestApp::Init()
{
	Scene* scene = this->GetScene();
	m_helmet = scene->LoadModel("../Resources/Models/DamagedHelmet/DamagedHelmet.gltf", "Helmet");
	m_helmet->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	m_helmet->SetRotation(glm::rotate(glm::quat(),
		glm::radians(90.0f), glm::vec3{ 1.0f, 0.0f, 0.0f }));

	m_avocado = scene->LoadModel("../Resources/Models/Avocado/Avocado.gltf", "Avocado");
	m_avocado->SetPosition(glm::vec3(2.0f, 0.0f, -1.f));
	m_avocado->SetScale(glm::vec3{ 15.0f, 15.0f, 15.0f });
	m_avocado->SetRotation(glm::rotate(glm::quat(),
		glm::radians(135.0f), glm::vec3{ 0.0f, 1.0f, 0.0f }));
	Material* avocadoMat = m_avocado->GetMaterial(0);
	avocadoMat->SetUseNormal(true);

	m_duck = scene->LoadModel("../Resources/Models/Duck/Duck.gltf", "Duck");
	m_duck->SetPosition(glm::vec3(-2.0f, -1.0f, 0.0f));
	m_duck->SetScale(glm::vec3{ 0.008f, 0.008f, 0.008f });
	m_duck->SetRotation(glm::rotate(glm::quat(),
		glm::radians(270.0f), glm::vec3{ 0.0f, 1.0f, 0.0f }));
	m_duckMat = m_duck->GetMaterial(0);
	m_duckMat->SetMetallicFactor(0.0f);
	m_duckMat->SetRoughnessFactor(0.0f);

	m_spheres = scene->LoadModel("../Resources/Models/MetalRoughSpheres/MetalRoughSpheres.gltf", "Spheres");
	m_spheres->SetPosition(glm::vec3(0.0f, -0.1f, -4.5f));
	m_spheres->SetScale(glm::vec3{ 0.2f, 0.2f, 0.2f });
	m_spheres->SetRotation(glm::rotate(glm::rotate(glm::quat(),
		glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
		glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
	
	m_madoka = scene->LoadModel("../Resources/Models/madoka_machida/scene.gltf", "Madoka");
	m_madoka->SetPosition(glm::vec3(-0.8f, -1.f, 2.0f));
	m_madoka->SetScale(glm::vec3(0.065f, 0.065f, 0.065f));
	m_madoka->SetRotation(glm::rotate(glm::quat(),
		glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)));

	m_mikuStand = scene->LoadModel("../Resources/Models/mikuStand/scene.gltf", "Miku Stand");
	m_mikuStand->SetPosition(glm::vec3(0.6f, -1.0f, 1.6f));
	m_mikuStand->SetScale(glm::vec3(0.3f, 0.3f, 0.3f));

	m_mainLight = scene->CreateLight("Main");
	m_mainLight->SetPosition(glm::vec3{ 0.0f, 2.0f, 1.0f });
	m_mainLight->SetRadiance(glm::vec3{ 5.0f });

	m_cam = scene->GetMainCamera();
	m_cam->SetPosition(glm::vec3(0.0f, 0.0f, 5.f));

	unsigned int halfWinWidth = this->GetWidth() / 2;
	unsigned int height = this->GetHeight();
	Viewport* mainViewport = m_cam->GetViewport();
	mainViewport->SetWidth(halfWinWidth);
	mainViewport->SetHeight(height);

	m_secondaryCam = scene->CreateCamera("SecondaryCam");
	m_secondaryCam->SetPosition(glm::vec3(1.0f, 2.0f, 3.0f));

	Viewport* secondaryViewport = m_secondaryCam->GetViewport();
	secondaryViewport->SetWidth(halfWinWidth);
	secondaryViewport->SetHeight(height);
	secondaryViewport->SetX(halfWinWidth);

	return true;
}

void TestApp::Update(float dt)
{
	glm::vec3 camPos = m_cam->GetPosition();
	if (m_bRotateCam)
	{
		m_elasedTime += dt;
		m_rotateRad += dt * 0.01f;
		camPos.x = m_rotateRad * glm::sin(m_elasedTime);
		camPos.z = m_rotateRad * glm::cos(m_elasedTime);
	}
	camPos.y = m_camY;
	m_cam->SetPosition(camPos);

	m_duckAngle += dt * m_duckRotatePower;
	m_duck->SetRotation(glm::rotate(glm::quat(),
		glm::radians(m_duckAngle), glm::vec3{ 0.0f, 1.0f, 0.0f }));

	m_duckRoughness += (dt * m_duckRoughnessScale);
	if (m_duckRoughness > 1.0f || m_duckRoughness < 0.0f)
	{
		m_duckRoughnessScale = -m_duckRoughnessScale;
	}
	m_duckMat->SetRoughnessFactor(m_duckRoughness);
}

void TestApp::WindowResizeCallback(GLFWwindow * window, int width, int height)
{
	unsigned int halfWinWidth = width / 2;
	Viewport* mainViewport = m_cam->GetViewport();
	mainViewport->SetWidth(halfWinWidth);
	mainViewport->SetHeight(height);

	Viewport* secondaryViewport = m_secondaryCam->GetViewport();
	secondaryViewport->SetWidth(halfWinWidth);
	secondaryViewport->SetHeight(height);
	secondaryViewport->SetX(halfWinWidth);

	std::cout << "Window resized: " << width << " , " << height << std::endl;
}

void TestApp::KeyCallback(GLFWwindow * window, int key, int scanCode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_SPACE:
			m_bRotateCam = !m_bRotateCam;
			break;

		case GLFW_KEY_UP:
			m_duckRotatePower += m_powerDiff;
			break;
		case GLFW_KEY_DOWN:
			m_duckRotatePower -= m_powerDiff;
			break;

		case GLFW_KEY_W:
			m_rotateRad -= m_rotateRadDiff;
			break;
		case GLFW_KEY_S:
			m_rotateRad += m_rotateRadDiff;
			break;
		case GLFW_KEY_A:
			m_camY -= m_camYDiff;
			break;
		case GLFW_KEY_D:
			m_camY += m_camYDiff;
			break;

		case GLFW_KEY_L:
			RandomLightGen();
			break;

		case GLFW_KEY_C:
			SplitViewport();
			break;

		}
	}
}

void TestApp::RandomLightGen()
{
	static unsigned int count = 0;
	static std::random_device device;
	static std::mt19937 engine(device());

	std::uniform_real_distribution<float> rad_dist(0.0f, 25.0f);
	std::uniform_real_distribution<float> pos_dist(-5.0f, 5.0f);

	auto* newLight = GetScene()->CreateLight("Random" + std::to_string(count));
	newLight->SetRadiance(glm::vec3(
		rad_dist(engine),
		rad_dist(engine),
		rad_dist(engine)));
	newLight->SetPosition(glm::vec3(
		pos_dist(engine),
		pos_dist(engine),
		pos_dist(engine)));
}

void TestApp::SplitViewport()
{
	bool status = !m_secondaryCam->IsActivated();

	unsigned int width = GetWidth();
	unsigned int height = GetHeight();
	unsigned int halfWinWidth = width / 2;
	Viewport* mainViewport = m_cam->GetViewport();
	Viewport* secondaryViewport = m_secondaryCam->GetViewport();

	if (status)
	{
		mainViewport->SetWidth(halfWinWidth);
		mainViewport->SetHeight(height);

		secondaryViewport->SetWidth(halfWinWidth);
		secondaryViewport->SetHeight(height);
		secondaryViewport->SetX(halfWinWidth);
	}
	else
	{
		mainViewport->SetWidth(width);
		mainViewport->SetHeight(height);
	}

	m_secondaryCam->SetActive(status);
}