#define NOMINMAX
#include "TestApp.h"
#include "Scene.h"
#include "Model.h"
#include "Camera.h"
#include "Material.h"
#include "Light.h"
#include "Viewport.h"
#include "Renderer.h"
#include "Controller.h"

#include "GLFW/glfw3.h"

#include <iostream>
#include <cmath>
#include <random>
#include <algorithm>

bool TestApp::Init()
{
	glfwSetInputMode(GetWindow(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	Scene* scene = this->GetScene();

	ModelLoadParams duckParams{
	.CalcTangentSpace = true,
	.ConvertToLeftHanded = true,
	.GenSmoothNormals = true,
	.GenUVs = true,
	.PreTransformVertices = true,
	.Triangulate = true };
	m_duck = scene->LoadModel("Duck", "Resources/Models/Duck/Duck.gltf", duckParams);
	m_duck->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	m_duck->SetRotation(glm::rotate(glm::quat(),
		glm::radians(270.0f), glm::vec3{ 0.0f, 1.0f, 0.0f }));
	m_duckMat = m_duck->GetMaterial(0);
	m_duckMat->SetMetallicFactor(0.0f);
	m_duckMat->SetRoughnessFactor(1.0f);

	ModelLoadParams sponzaLoadParams{
	   .CalcTangentSpace = true,
	   .ConvertToLeftHanded = false,
	   .GenSmoothNormals=false,
	   .GenUVs = true,
	   .PreTransformVertices=false,
	   .Triangulate=false};
	m_sponza = scene->LoadModel("Sponza", "Resources/Models/Sponza/Sponza.gltf", sponzaLoadParams);
	m_sponza->SetScale(glm::vec3(0.09f, 0.09f, 0.09f));

	/*m_helmet = scene->LoadModel("../Resources/Models/DamagedHelmet/DamagedHelmet.gltf", "Helmet");
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
	m_mikuStand->SetScale(glm::vec3(0.3f, 0.3f, 0.3f));*/

	m_mainLight = scene->CreateLight("Main");
	m_mainLight->SetPosition(glm::vec3{ 0.0f, 2.0f, 1.0f });
	m_mainLight->SetRadiance(glm::vec3{ 5.0f });

	m_cam = scene->GetMainCamera();
	m_cam->SetPosition(glm::vec3(0.0f, 0.0f, 0.f));
	m_controller = new Controller(m_cam, this->GetWindow());

	unsigned int width = this->GetWidth();
	unsigned int height = this->GetHeight();
	Viewport* mainViewport = m_cam->GetViewport();
	mainViewport->SetWidth(width);
	mainViewport->SetHeight(height);

	return true;
}

void TestApp::Update(float dt)
{
	m_controller->Update(dt);
}

void TestApp::WindowResizeCallback(GLFWwindow * window, int width, int height)
{
	Viewport* mainViewport = m_cam->GetViewport();
	mainViewport->SetWidth(width);
	mainViewport->SetHeight(height);

	std::cout << "Window resized: " << width << " , " << height << std::endl;
}

void TestApp::KeyCallback(GLFWwindow * window, int key, int scanCode, int action, int mods)
{
	if (m_controller != nullptr)
	{
		m_controller->KeyCallback(window, key, scanCode, action, mods);
	}

	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_L:
			RandomLightGen();
			break;

		case GLFW_KEY_C:
			//SplitViewport();
			break;
		case GLFW_KEY_ESCAPE:
			this->Stop();
			break;

		}
	}
}

void TestApp::RandomLightGen()
{
	static unsigned int count = 0;
	static std::random_device device;
	static std::mt19937 engine(device());

	std::uniform_real_distribution<float> rad_dist(0.0f, 60.0f);
	std::uniform_real_distribution<float> pos_dist(-100.0f, 100.0f);

	auto* newLight = GetScene()->CreateLight("Random" + std::to_string(count));
	newLight->SetRadiance(glm::vec3(
		rad_dist(engine),
		rad_dist(engine),
		rad_dist(engine)));
	newLight->SetPosition(glm::vec3(
		pos_dist(engine),
		std::max(0.0f, pos_dist(engine)),
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