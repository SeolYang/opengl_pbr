#include "TestApp.h"
#include "Scene.h"
#include "Model.h"
#include "Camera.h"

bool TestApp::Init()
{
	Scene* scene = this->GetScene();
	m_helmet = scene->LoadModel("../Resources/Models/DamagedHelmet/DamagedHelmet.gltf", "Helmet");
	m_helmet->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	m_helmet->SetRotation(glm::rotate(glm::quat(),
		glm::radians(90.0f), glm::vec3{ 1.0f, 0.0f, 0.0f }));

	m_cam = scene->GetMainCamera();
	m_cam->SetPosition(glm::vec3(0.0f, 2.0f, 5.0f));
	return true;
}

void TestApp::Update(float dt)
{
	m_elasedTime += dt;
	glm::vec3 camPos = m_cam->GetPosition();
	camPos.x = m_rotateRad * glm::sin(m_elasedTime);
	camPos.z = m_rotateRad * glm::cos(m_elasedTime);
	m_cam->SetPosition(camPos);
}