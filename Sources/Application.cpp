#include "Application.h"
#include "Renderer.h"
#include "Scene.h"
#include "Viewport.h"

#include "GLFW/glfw3.h"

#include <chrono>
#include <iostream>

Application::Application(const std::string& title, unsigned int width, unsigned int height) :
	m_bIsRunning(false),
	m_title(title),
	m_scene(nullptr),
	m_renderer(nullptr),
	m_mainViewport(0)
{
	this->CreateViewport(0, 0, width, height);
}

Application::~Application()
{
	if (m_scene != nullptr)
	{
		delete m_scene;
		m_scene = nullptr;
	}
	if (m_renderer != nullptr)
	{
		delete m_renderer;
		m_renderer = nullptr;
	}

	for (auto viewport : m_viewports)
	{
		if (viewport != nullptr)
		{
			delete viewport;
		}
	}
}

bool Application::InitBase()
{
	m_scene = new Scene();
	m_renderer = new Renderer();

	if (!m_renderer->Init())
	{
		std::cout << "Failed to init renderer" << std::endl;
		return false;
	}

	return true;
}

bool Application::InitWindows()
{
	//GLFWwindow* window = glfwCreateWindow()
	return true;
}

int Application::Run()
{
	if (!InitBase())
	{
		return 1;
	}
	if (!InitWindows())
	{
		return 1;
	}
	if (!Init())
	{
		return 1;
	}

	if (m_viewports.size() > 0)
	{
		std::cout << "Error: Cannot find viewport" << std::endl;
		return 1;
	}

	m_bIsRunning = true;
	float deltaTime = 0.0f;
	while (m_bIsRunning && !glfwWindowShouldClose(m_window))
	{
		auto begin = std::chrono::system_clock::now();

		Update(deltaTime);
		m_renderer->Render(m_scene, m_viewports[m_mainViewport]);

		auto end = std::chrono::system_clock::now();
		std::chrono::duration<float> dt = (end - begin);
		deltaTime = dt.count();
	}

	return 0;
}

Viewport* Application::CreateViewport(unsigned int width, unsigned int height, unsigned int x, unsigned int y)
{
	Viewport* newViewport = new Viewport(width, height, x, y);
	m_viewports.push_back(newViewport);
	return newViewport;
}