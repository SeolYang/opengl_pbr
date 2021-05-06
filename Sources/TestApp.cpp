#define NOMINMAX
#include "TestApp.h"
#include "Renderer.h"
#include "Controller.h"
#include "Viewport.h"

#include "SponzaScene.h"

#include <iostream>

#include "Scene.h"

TestApp::~TestApp()
{
	delete m_controller;
	delete m_sponzaScene;
}

bool TestApp::Init()
{
	glfwSetInputMode(GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	m_sponzaScene = new SponzaScene();
	m_sponzaScene->Construct();
	this->SetScene(m_sponzaScene);

	m_mainScene = m_sponzaScene;

	Camera* sceneMainCamera = m_mainScene->GetMainCamera();
	unsigned int width = this->GetWidth();
	unsigned int height = this->GetHeight();
	Viewport* mainViewport = sceneMainCamera->GetViewport();
	mainViewport->SetWidth(width);
	mainViewport->SetHeight(height);

	m_controller = new Controller(sceneMainCamera, this->GetWindow());
	m_controller->CameraSpeed = 15.0f;
	m_controller->SetHorizontalAngle(glm::radians(-90.0f));
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
			break;

		case GLFW_KEY_1:
			renderer->bEnableDirectDiffuse = !renderer->bEnableDirectDiffuse;
			break;

		case GLFW_KEY_2:
			renderer->bEnableIndirectDiffuse = !renderer->bEnableIndirectDiffuse;
			break;

		case GLFW_KEY_3:
			renderer->bEnableDirectSpecular = !renderer->bEnableDirectSpecular;
			break;

		case GLFW_KEY_4:
			renderer->bEnableIndirectSpecular = !renderer->bEnableIndirectSpecular;
			break;

		case GLFW_KEY_5:
			renderer->bDebugAmbientOcclusion = !renderer->bDebugAmbientOcclusion;
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
				break;

			case ERenderMode::VoxelVisualization:
				renderer->SetRenderMode(ERenderMode::Deferred);
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
				break;

			case ERenderMode::Deferred:
				renderer->SetRenderMode(ERenderMode::VoxelVisualization);
				break;

			default:
				break;
			}
			break;

		case GLFW_KEY_V:
			this->GetRenderer()->SetVoxelizeEveryFrame(!this->GetRenderer()->IsVoxelizeEveryFrame());
			break;

		}
	}
}
