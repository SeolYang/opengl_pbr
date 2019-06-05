#include "Application.h"
#include "Renderer.h"
#include "Scene.h"
#include "Viewport.h"

#include "GL/gl3w.h"
#include "GLFW/glfw3.h"

#include <chrono>
#include <iostream>

Application::Application(const std::string& title, unsigned int width, unsigned int height) :
	m_bIsRunning(false),
	m_title(title),
	m_scene(nullptr),
	m_renderer(nullptr),
	m_windowWidth(width),
	m_windowHeight(height)
{
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
	glfwInit();
	const char* glslVersion = "#version 330 core";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	m_window = glfwCreateWindow(m_windowWidth, m_windowHeight,
		m_title.c_str(), nullptr, nullptr);
	if (m_window == nullptr)
	{
		std::cout << "Failed to init glfw window" << std::endl;
		return false;
	}

	glfwMakeContextCurrent(m_window);

	/* Initialize Callbacks */
	glfwSetWindowUserPointer(m_window, this);

	const auto keyCallback = [](GLFWwindow * window, int key, int scanCode, int action, int mods) {
		auto* app = (Application*)glfwGetWindowUserPointer(window);
		app->_KeyCallback(window, key, scanCode, action, mods); };
	glfwSetKeyCallback(m_window, keyCallback);

	const auto framebufferSizeCallback = [](GLFWwindow * window, int width, int height) {
		auto* app = (Application*)glfwGetWindowUserPointer(window);
		app->_WindowResizeCallback(window, width, height);
	};
	glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);

	int err = gl3wInit();
	int version = gl3wIsSupported(3, 3);

	return true;
}

int Application::Run()
{
	if (!InitWindows())
	{
		return 1;
	}
	if (!InitBase())
	{
		return 1;
	}
	if (!Init())
	{
		return 1;
	}

	m_bIsRunning = true;
	float deltaTime = 0.0f;
	while (m_bIsRunning && !glfwWindowShouldClose(m_window))
	{
		auto begin = std::chrono::system_clock::now();

		Update(deltaTime);
		m_renderer->Render(m_scene);

		glfwSwapBuffers(m_window);
		glfwPollEvents();

		auto end = std::chrono::system_clock::now();
		std::chrono::duration<float> dt = (end - begin);
		deltaTime = dt.count();
	}

	return 0;
}

void Application::_WindowResizeCallback(GLFWwindow* window, int width, int height)
{
	m_windowWidth = width;
	m_windowHeight = height;
	WindowResizeCallback(window, width, height);
}

void Application::_KeyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods)
{
	KeyCallback(window, key, scanCode, action, mods);
}