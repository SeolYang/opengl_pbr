#pragma once
#include "Application.h"

enum class EPredefinedScene
{
   Sponza,
	CornellBox
};

class Controller;
class SponzaScene;
class SanMiguelScene;
class CornellBoxScene;
class TestApp : public Application
{
public:
	TestApp(const std::string& title,
		unsigned int width, unsigned int height, bool bFullScreen) :
		Application(title, width, height, bFullScreen)
	{
	}

	~TestApp();

protected:
	bool Init() override;
	void Update(float dt) override;
	void KeyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods) override;

private:
	void ChangeSceneTo(EPredefinedScene scene);

private:
	Controller* m_controller = nullptr;

	/* Scenes */
	Scene* m_mainScene = nullptr;
	SponzaScene* m_sponzaScene = nullptr;
	CornellBoxScene* m_cornellBoxScene = nullptr;
	SanMiguelScene* m_sanMiguelScene = nullptr;

};
