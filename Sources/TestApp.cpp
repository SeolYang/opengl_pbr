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

	m_avocado = scene->LoadModel("../Resources/Models/Avocado/Avocado.gltf", "Avocado");
	m_avocado->SetPosition(glm::vec3(2.0f, 0.0f, 0.0f));
	m_avocado->SetScale(glm::vec3{ 15.0f, 15.0f, 15.0f });
	m_avocado->SetRotation(glm::rotate(glm::quat(),
		glm::radians(135.0f), glm::vec3{ 0.0f, 1.0f, 0.0f }));


	m_duck = scene->LoadModel("../Resources/Models/Duck/Duck.gltf", "Duck");
	m_duck->SetPosition(glm::vec3(-2.0f,-1.0f, 0.0f));
	m_duck->SetScale(glm::vec3{ 0.008f, 0.008f, 0.008f });
	m_duck->SetRotation(glm::rotate(glm::quat(),
		glm::radians(270.0f), glm::vec3{ 0.0f, 1.0f, 0.0f }));

	m_metalRoughSpheres = scene->LoadModel("../Resources/Models/MetalRoughSpheres/MetalRoughSpheres.gltf", "Spheres");
	m_metalRoughSpheres->SetPosition(glm::vec3(0.0f, 0.0f, -1.5f));
	m_metalRoughSpheres->SetScale(glm::vec3(0.5f));
	m_metalRoughSpheres->SetRotation(glm::rotate(glm::quat(),
		glm::radians(-90.0f), glm::vec3{ 0.0f, 1.0f, 0.0f }));

	m_cam = scene->GetMainCamera();
	m_cam->SetPosition(glm::vec3(0.0f, 3.0f, 8.0f));
	return true;
}

void TestApp::Update(float dt)
{
	m_elasedTime += dt;
	m_duckAngle += dt * m_rotatePower;
	glm::vec3 camPos = m_duck->GetPosition();
	//m_rotateRad += dt*0.01f;
	camPos.x = m_rotateRad * glm::sin(m_elasedTime);
	camPos.z = m_rotateRad * glm::cos(m_elasedTime);
	//m_cam->SetPosition(camPos);
	m_duck->SetRotation(glm::rotate(glm::quat(),
		glm::radians(m_duckAngle), glm::vec3{ 0.0f, 1.0f, 0.0f }));
}