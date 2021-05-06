#pragma once
#include "Application.h"

class Controller;
class SponzaScene;
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
	Controller* m_controller = nullptr;

	/* Scenes */
	Scene* m_mainScene = nullptr;
	SponzaScene* m_sponzaScene = nullptr;

};
