#include "TestApp.h"
#include "Scene.h"
#include "Model.h"
#include "Camera.h"
#include "Material.h"
#include "Light.h"

#include <iostream>

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
	Material* avocadoMat = m_avocado->GetMaterial(0);
	avocadoMat->SetUseNormal(false);


	m_duck = scene->LoadModel("../Resources/Models/Duck/Duck.gltf", "Duck");
	m_duck->SetPosition(glm::vec3(-2.0f,-1.0f, 0.0f));
	m_duck->SetScale(glm::vec3{ 0.008f, 0.008f, 0.008f });
	m_duck->SetRotation(glm::rotate(glm::quat(),
		glm::radians(270.0f), glm::vec3{ 0.0f, 1.0f, 0.0f }));
	m_duckMat = m_duck->GetMaterial(0);
	m_duckMat->SetMetallicFactor(0.0f);
	m_duckMat->SetRoughnessFactor(0.0f);

	m_spheres = scene->LoadModel("../Resources/Models/MetalRoughSpheres/MetalRoughSpheres.gltf", "Spheres");
	m_spheres->SetPosition(glm::vec3(0.0f, -1.f, -5.5f));
	m_spheres->SetScale(glm::vec3{ 0.3f, 0.3f, 0.3f });
	m_spheres->SetRotation(glm::rotate(glm::quat(),
		glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

	m_mainLight = scene->CreateLight("Main");
	m_mainLight->SetPosition(glm::vec3{ 0.0f, 2.0f, 1.0f });
	m_mainLight->SetRadiance(glm::vec3{ 25.0f });

	m_secondaryLight = scene->CreateLight("Secondary");
	m_secondaryLight->SetPosition(glm::vec3{ -1.5f, 3.0f, -2.5f });
	m_secondaryLight->SetRadiance(glm::vec3{ 30.0f, 5.0f, 5.0f });

	m_cam = scene->GetMainCamera();
	m_cam->SetPosition(glm::vec3(0.0f, 0.0f, 5.f));
	return true;
}

void TestApp::Update(float dt)
{
	m_elasedTime += dt;

	m_rotateRad += dt*0.01f;
	glm::vec3 camPos = m_cam->GetPosition();
	camPos.x = m_rotateRad * glm::sin(m_elasedTime);
	camPos.z = m_rotateRad * glm::cos(m_elasedTime);
	m_cam->SetPosition(camPos);

	m_duckAngle += dt * m_rotatePower;
	m_duck->SetRotation(glm::rotate(glm::quat(),
		glm::radians(m_duckAngle), glm::vec3{ 0.0f, 1.0f, 0.0f }));

	m_duckRoughness += (dt * m_duckRoughnessScale);
	if (m_duckRoughness > 1.0f || m_duckRoughness < 0.0f)
	{
		m_duckRoughnessScale = -m_duckRoughnessScale;
	}
	m_duckMat->SetRoughnessFactor(m_duckRoughness);
}