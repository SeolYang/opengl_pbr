#define NOMINMAX
#include "TestApp.h"
#include "Renderer.h"
#include "Controller.h"
#include "Viewport.h"

#include "SponzaScene.h"
#include "CornellBoxScene.h"

#include <iostream>

#include "Scene.h"

TestApp::~TestApp()
{
	delete m_controller;
	delete m_sponzaScene;
	delete m_cornellBoxScene;
}

bool TestApp::Init()
{
	glfwSetInputMode(GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	m_sponzaScene = new SponzaScene();
	{
		m_sponzaScene->Construct();

		Camera* sceneMainCamera = m_sponzaScene->GetMainCamera();
		Viewport* mainViewport = sceneMainCamera->GetViewport();
		unsigned int width = this->GetWidth();
		unsigned int height = this->GetHeight();
		mainViewport->SetWidth(width);
		mainViewport->SetHeight(height);
	}

	m_cornellBoxScene = new CornellBoxScene();
	{
		m_cornellBoxScene->Construct();

		Camera* sceneMainCamera = m_cornellBoxScene->GetMainCamera();
		Viewport* mainViewport = sceneMainCamera->GetViewport();
		unsigned int width = this->GetWidth();
		unsigned int height = this->GetHeight();
		mainViewport->SetWidth(width);
		mainViewport->SetHeight(height);
	}

	this->SetScene(m_sponzaScene);
	m_mainScene = m_sponzaScene;
	Camera* mainSceneCamera = m_mainScene->GetMainCamera();
	m_controller = new Controller(mainSceneCamera, this->GetWindow());
	m_controller->CameraSpeed = 15.0f;
	m_controller->SetHorizontalAngle(glm::radians(-90.0f));
	ChangeSceneTo(EPredefinedScene::Sponza);
	return true;
}

void TestApp::Update(float dt)
{
	m_controller->Update(dt);
}

void TestApp::KeyCallback(GLFWwindow * window, int key, int scanCode, int action, int mods)
{
	m_controller->KeyCallback(window, key, scanCode, action, mods);
	if (action == GLFW_PRESS)
	{
		auto renderer = this->GetRenderer();
		switch (key)
		{
		default:
			break;

		case GLFW_KEY_F3:
			renderer->bEnableConservativeRasterization = !renderer->bEnableConservativeRasterization;
			if (renderer->bEnableConservativeRasterization)
			{
				std::cout << "Renderer : Enabled Conservative Rasterization based Voxelization" << std::endl;
			}
			else
			{
				std::cout << "Renderer : Disabled Conservative Rasterization based Voxelization" << std::endl;
			}
			break;

		case GLFW_KEY_F5:
			ChangeSceneTo(EPredefinedScene::Sponza);
			break;

		case GLFW_KEY_F6:
			ChangeSceneTo(EPredefinedScene::CornellBox);
			break;

		case GLFW_KEY_N:
			renderer->bDebugConeDirection = !renderer->bDebugConeDirection;
			if (renderer->bDebugConeDirection)
			{
				std::cout << "Renderer : Enabled Diffuse Cones Direction Debug" << std::endl;
			}
			else
			{
				std::cout << "Renderer : Disabled Diffuse Cones Direction Debug" << std::endl;
			}
			break;

		case GLFW_KEY_1:
			renderer->bEnableDirectDiffuse = !renderer->bEnableDirectDiffuse;
			if (renderer->bEnableDirectDiffuse)
			{
				std::cout << "Renderer : Enabled Direct Diffuse" << std::endl;
			}
			else
			{
				std::cout << "Renderer : Disabled Direct Diffuse" << std::endl;
			}
			break;

		case GLFW_KEY_2:
			renderer->bEnableIndirectDiffuse = !renderer->bEnableIndirectDiffuse;
			if (renderer->bEnableIndirectDiffuse)
			{
				std::cout << "Renderer : Enabled Indirect Diffuse" << std::endl;
			}
			else
			{
				std::cout << "Renderer : Disabled Indirect Diffuse" << std::endl;
			}
			break;

		case GLFW_KEY_3:
			renderer->bEnableDirectSpecular = !renderer->bEnableDirectSpecular;
			if (renderer->bEnableDirectSpecular)
			{
				std::cout << "Renderer : Enabled Direct Specular" << std::endl;
			}
			else
			{
				std::cout << "Renderer : Disabled Direct Specular" << std::endl;
			}
			break;

		case GLFW_KEY_4:
			renderer->bEnableIndirectSpecular = !renderer->bEnableIndirectSpecular;
			if (renderer->bEnableIndirectSpecular)
			{
				std::cout << "Renderer : Enabled Indirect Specular" << std::endl;
			}
			else
			{
				std::cout << "Renderer : Disabled Indirect Specular" << std::endl;
			}
			break;

		case GLFW_KEY_5:
			renderer->bDebugAmbientOcclusion = !renderer->bDebugAmbientOcclusion;
			if (renderer->bDebugAmbientOcclusion)
			{
				std::cout << "Renderer : Enabled Ambient Occlusion Debug" << std::endl;
			}
			else
			{
				std::cout << "Renderer : Disabled Ambient Occlusion Debug" << std::endl;
			}
			break;

		case GLFW_KEY_PAGE_UP:
			renderer->VCTSpecularSampleNum = std::min<unsigned int>(64, renderer->VCTSpecularSampleNum * 2);
			std::cout << "Indirect Specular Samples : " << renderer->VCTSpecularSampleNum << std::endl;
			break;

		case GLFW_KEY_PAGE_DOWN:
			renderer->VCTSpecularSampleNum = std::max<unsigned int>(1, renderer->VCTSpecularSampleNum / 2);
			std::cout << "Indirect Specular Samples : " << renderer->VCTSpecularSampleNum << std::endl;
			break;

		case GLFW_KEY_ESCAPE:
			this->Stop();
			break;

		case GLFW_KEY_LEFT_BRACKET:
			switch(renderer->GetRenderMode())
			{
			case ERenderMode::Deferred:
				renderer->SetRenderMode(ERenderMode::VCT);
				std::cout << "Voxel Cone Tracing Mode" << std::endl;
				break;

			case ERenderMode::VoxelVisualization:
				renderer->SetRenderMode(ERenderMode::Deferred);
				std::cout << "Deferred Rendering Mode" << std::endl;
				break;

			default:
				break;
			}
			break;

		case GLFW_KEY_RIGHT_BRACKET:
			switch (renderer->GetRenderMode())
			{
			case ERenderMode::VCT:
				renderer->SetRenderMode(ERenderMode::Deferred);
				std::cout << "Deferred Rendering Mode" << std::endl;
				break;

			case ERenderMode::Deferred:
				renderer->SetRenderMode(ERenderMode::VoxelVisualization);
				std::cout << "Voxel Visualization Mode" << std::endl;
				break;

			default:
				break;
			}
			break;

		case GLFW_KEY_V:
			renderer->bAlwaysVoxelize = !renderer->bAlwaysVoxelize;
			if (renderer->bAlwaysVoxelize)
			{
				std::cout << "Renderer : Always Voxelize!" << std::endl;
			}
			else
			{
				std::cout << "Renderer : Voxelize on scene changed!" << std::endl;
			}
			break;

		}
	}
}

void TestApp::ChangeSceneTo(EPredefinedScene scene)
{
	const auto renderer = this->GetRenderer();
	switch(scene)
	{
	case EPredefinedScene::Sponza:
		m_mainScene = m_sponzaScene;
		renderer->DebugConeLength = 0.1f;
		renderer->VCTInitialStep = 3.5f;
		std::cout << "Application : Changed Main Scene to Sponza" << std::endl;
		break;

	case EPredefinedScene::CornellBox:
		m_mainScene = m_cornellBoxScene;
		renderer->DebugConeLength = 1.5f;
		renderer->VCTInitialStep = 8.5f;
		std::cout << "Application : Changed Main Scene to Cornell Box" << std::endl;
		break;
	}

	this->SetScene(m_mainScene);
	m_controller->SetTarget(m_mainScene->GetMainCamera());
	m_mainScene->SetToDirty();
	renderer->PrintVCTParams();
}
