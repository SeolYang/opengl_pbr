#pragma once
#include <string>
#include <vector>

class Scene;
class Renderer;
struct GLFWwindow;
class Viewport;
class Application
{
public:
	Application(const std::string& title, unsigned int width, unsigned int height);
	~Application();

	std::string GetTitle() const { return m_title; }

	int Run();
	void Stop() { m_bIsRunning = false; }

	Scene* GetScene() const { return m_scene; }
	Renderer* GetRenderer() const { return m_renderer; }

	unsigned int GetWidth() const { return m_windowWidth; }
	unsigned int GetHeight() const { return m_windowHeight; }

protected:
	virtual bool Init() = 0;
	virtual void Update(float dt) = 0;

	virtual void WindowResizeCallback(GLFWwindow* window, int width, int height) {}
	virtual void KeyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods) {}

private:
	void _WindowResizeCallback(GLFWwindow* window, int width, int height);
	void _KeyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods);

private:
	bool InitBase();
	bool InitWindows();

private:
	bool m_bIsRunning;

	std::string m_title;
	GLFWwindow* m_window;

	Scene* m_scene;
	Renderer* m_renderer;

	unsigned int m_windowWidth;
	unsigned int m_windowHeight;

};