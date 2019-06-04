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

	Viewport* CreateViewport(unsigned int width, unsigned int height, unsigned int x = 0, unsigned int y = 0);
	void SetMainViewport(unsigned int idx) { m_mainViewport = idx; }
	Viewport* GetMainViewport() const { return m_viewports[m_mainViewport]; }

	Scene* GetScene() const { return m_scene; }
	Renderer* GetRenderer() const { return m_renderer; }

protected:
	virtual bool Init() = 0;
	virtual void Update(float dt) = 0;

	virtual void WindowResizeCallback(GLFWwindow* window, int width, int height) {}
	virtual void KeyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods) {}

private:
	bool InitBase();
	bool InitWindows();

private:
	bool m_bIsRunning;

	std::string m_title;
	GLFWwindow* m_window;

	Scene* m_scene;
	Renderer* m_renderer;

	std::vector<Viewport*> m_viewports;
	unsigned int m_mainViewport;

};