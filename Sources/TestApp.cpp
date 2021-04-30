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
#include "Material.h"

#include "GLFW/glfw3.h"

#include <iostream>
#include <cmath>
#include <random>
#include <algorithm>

#include "Texture2D.h"

bool TestApp::Init()
{
	glfwSetInputMode(GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	Scene* scene = this->GetScene();
	ModelLoadParams duckParams{
	.CalcTangentSpace = true,
	.ConvertToLeftHanded = true,
	.GenSmoothNormals = true,
	.GenUVs = true,
	.PreTransformVertices = true,
	.Triangulate = true };
	m_duck = scene->LoadModel("Duck", "Resources/Models/Duck/Duck.gltf", duckParams);
	m_duck->SetPosition(glm::vec3(-0.05f, 0.0f, 0.0f));
	m_duck->SetScale(glm::vec3(2.0f));
	m_duck->SetRotation(glm::rotate(glm::quat(),
		glm::radians(180.0f), glm::vec3{ 0.0f, 1.0f, 0.0f }));
	m_duckMat = m_duck->GetMaterial(0);
	m_duckMat->SetMetallicFactor(0.0f);
	m_duckMat->SetRoughnessFactor(0.0f);

	ModelLoadParams sponzaLoadParams{
	   .CalcTangentSpace = true,
	   .ConvertToLeftHanded = true,
	   .GenSmoothNormals=false,
	   .GenUVs = true,
	   .PreTransformVertices=false,
	   .Triangulate=false};
	m_sponza = scene->LoadModel("Sponza", "Resources/Models/Sponza/Sponza.gltf", sponzaLoadParams);
	m_sponza->SetScale(glm::vec3(0.045f));

	const auto& sponzaMaterials = m_sponza->GetMaterials();
	for (auto material : sponzaMaterials)
	{
		std::string_view baseColorPath = material->GetBaseColor()->GetURI();
		// Lion : Gold
		if (baseColorPath == "Resources/Models/Sponza/6772804448157695701.jpg")
		{
			material->SetForceFactor(EMaterialTexture::MetallicRoughness, true);
			material->SetRoughnessFactor(0.2f);
			material->SetMetallicFactor(0.9f);

			material->SetForceFactor(EMaterialTexture::BaseColor, true);
			material->SetBaseColorFactor(glm::vec4(1.0f, 0.766f, 0.336f, 1.0f));
		}

		// Floor : Chromium
		//if (baseColorPath == "Resources/Models/Sponza/5823059166183034438.jpg")
		//{
		//	material->SetForceFactor(EMaterialTexture::MetallicRoughness, true);
		//	material->SetRoughnessFactor(0.4f);
		//	material->SetMetallicFactor(0.97f);

		//	material->SetForceFactor(EMaterialTexture::BaseColor, true);
		//	material->SetBaseColorFactor(glm::vec4(0.55f, 0.556f, 0.554f, 1.0f));
		//}
	}

	//ModelLoadParams cornellParams
 //  {
	//		.CalcTangentSpace = true,
	//		.ConvertToLeftHanded = true,
	//		.GenSmoothNormals = false,
	//		.GenUVs = true,
	//		.PreTransformVertices = false,
	//		.Triangulate = false
 //  };

	//Model* cornell = scene->LoadModel("CornellBox", "Resources/Models/cornell.obj", cornellParams);
	//cornell->SetScale(glm::vec3(0.9f));

	//glm::vec4 cornellColor[7] = {
	//	glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
	//	glm::vec4(1.0f),
	//	glm::vec4(1.0f),
	//	glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
	//	glm::vec4(1.0f),
	//	glm::vec4(1.0f),
	//	glm::vec4(1.0f)
	//};

	//auto cornellMaterials = cornell->GetMaterials();
	//for (size_t idx = 0; idx < cornellMaterials.size(); ++idx)
	//{
	//	cornellMaterials[idx]->SetBaseColorFactor(cornellColor[idx]);
	//}

	m_mainLight = scene->CreateLight("Main");
	m_mainLight->SetPosition(glm::vec3{ 0.0f, 0.0f, 0.0f });
	m_mainLight->SetIntensity(glm::vec3{ 3.0f });
	this->UpdateLightRotation();

	m_cam = scene->GetMainCamera();
	m_cam->SetPosition(glm::vec3(0.0f, 0.05f, 0.f));
	m_controller = new Controller(m_cam, this->GetWindow());
	m_controller->CameraSpeed = 15.0f;

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

		case GLFW_KEY_ESCAPE:
			this->Stop();
			break;

		case GLFW_KEY_M:
			this->GetRenderer()->SetRenderMode(this->GetRenderer()->GetRenderMode() == ERenderMode::Deferred ? ERenderMode::VoxelVisualization : ERenderMode::Deferred);
			break;

		case GLFW_KEY_R:
			this->m_cam->SetPosition(glm::vec3(0.0f));
			break;

		case GLFW_KEY_V:
			this->GetRenderer()->SetVoxelizeEveryFrame(!this->GetRenderer()->IsVoxelizeEveryFrame());
			break;

		case GLFW_KEY_RIGHT:
			m_lightRotationX += 5.0f;
			break;

		case GLFW_KEY_LEFT:
			m_lightRotationX -= 5.0f;
			break;

		case GLFW_KEY_DOWN:
			m_lightRotationY -= 10.0f;
			break;

		case GLFW_KEY_UP:
			m_lightRotationY += 10.0f;
			break;
		}

		this->UpdateLightRotation();
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
	newLight->SetIntensity(glm::vec3(
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

void TestApp::UpdateLightRotation()
{
	const glm::quat lightXRot = glm::rotate(glm::quat(),
		glm::radians(m_lightRotationX), glm::vec3{ 1.0f, 0.0f, 0.0f });

	const glm::quat lightYRot = glm::rotate(glm::quat(),
		glm::radians(m_lightRotationY), glm::vec3{ 0.0f, 1.0f, 0.0f });

	m_mainLight->SetRotation(lightXRot* lightYRot);

	std::cout << "Light rotation X : " << m_lightRotationX << " Y : " << m_lightRotationY << std::endl;
}
