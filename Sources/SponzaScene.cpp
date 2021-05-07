#define NOMINMAX
#include "SponzaScene.h"
#include "Model.h"
#include "Camera.h"
#include "Material.h"
#include "Light.h"
#include "Controller.h"
#include "Texture2D.h"

#include <iostream>
#include <cmath>
#include <random>
#include <algorithm>
#include <filesystem>
#include <glm/gtx/quaternion.hpp>

void SponzaScene::Construct()
{
	const ModelLoadParams sponzaLoadParams{
		.CalcTangentSpace = true,
		.ConvertToLeftHanded = true,
		.GenSmoothNormals = true,
		.GenUVs = false,
		.PreTransformVertices = false,
		.Triangulate = false };
	m_sponza = this->LoadModel("Sponza", "Resources/Models/Sponza/Sponza.gltf", sponzaLoadParams);
	m_sponza->SetScale(glm::vec3(0.05f));

	const auto& sponzaMaterials = m_sponza->GetMaterials();
	for (auto material : sponzaMaterials)
	{
		const std::string_view baseColorPath = material->GetBaseColor()->GetURI();
		//Floor
		if (baseColorPath == "Resources/Models/Sponza/5823059166183034438.jpg")
		{
			m_floorMaterial = material;
			material->SetForceFactor(EMaterialTexture::MetallicRoughness, true);
			material->SetRoughnessFactor(m_floorRoughness);
			material->SetMetallicFactor(0.0f);
		}
	}

	const ModelLoadParams helmetLoadParams{
		.CalcTangentSpace = true,
		.ConvertToLeftHanded = true,
		.GenSmoothNormals = true,
		.GenUVs = false,
		.PreTransformVertices = true,
		.Triangulate = false };
	auto helmet = this->LoadModel("Helmet", "Resources/Models/DamagedHelmet/DamagedHelmet.gltf", helmetLoadParams);
	helmet->SetPosition(glm::vec3(0.0f, 2.0f, -5.5f));
	helmet->SetScale(glm::vec3(2.5f));
	helmet->SetRotation(glm::rotate(glm::quat(), glm::radians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
	helmet->GetMaterial(0)->SetEmissiveIntensity(5.0f);

	const ModelLoadParams sphereLoadParams{
		.CalcTangentSpace = true,
		.ConvertToLeftHanded = false,
		.GenSmoothNormals = true,
		.GenUVs = true,
		.PreTransformVertices = true,
		.Triangulate = true
	};
	m_sphere = this->LoadModel("Sphere", "Resources/Models/sphere.obj", sphereLoadParams);
	m_sphere->bCastShadow = false;
	m_sphere->SetActive(false);
	auto sphereMat = m_sphere->GetMaterial(0);
	//sphereMat->SetForceFactor(EMaterialTexture::Emissive, true);
	sphereMat->SetForceFactor(EMaterialTexture::BaseColor, true);
	sphereMat->SetBaseColorFactor(glm::vec4(0.0f));
	//sphereMat->SetBaseColorFactor(glm::vec4(1.0f));
	sphereMat->bRefract = true;
	sphereMat->IOR = 1.2f;

	auto emissiveSphere0 = this->LoadModel("EmissiveSphere0", "Resources/Models/sphere.obj", sphereLoadParams);
	emissiveSphere0->SetPosition(glm::vec3(20.0f, 30.0f, -20.0f));
	auto emissiveSphere0Mat = emissiveSphere0->GetMaterial(0);

	emissiveSphere0Mat->SetForceFactor(EMaterialTexture::BaseColor, true);
	emissiveSphere0Mat->SetForceFactor(EMaterialTexture::Emissive, true);
	emissiveSphere0Mat->SetEmissiveFactor(glm::vec3(0.0f, 1.0f, 0.0f));
	emissiveSphere0Mat->SetEmissiveIntensity(1.0f);
	emissiveSphere0Mat->SetBaseColorFactor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

	auto emissiveSphere1 = this->LoadModel("EmissiveSphere1", "Resources/Models/sphere.obj", sphereLoadParams);
	emissiveSphere1->SetPosition(glm::vec3(0.0f, 30.0f, -20.0f));
	auto emissiveSphere1Mat = emissiveSphere1->GetMaterial(0);

	emissiveSphere1Mat->SetForceFactor(EMaterialTexture::BaseColor, true);
	emissiveSphere1Mat->SetForceFactor(EMaterialTexture::Emissive, true);
	emissiveSphere1Mat->SetEmissiveFactor(glm::vec3(0.0f, 1.0f, 1.0f));
	emissiveSphere1Mat->SetEmissiveIntensity(1.0f);
	emissiveSphere1Mat->SetBaseColorFactor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

	auto emissiveSphere2 = this->LoadModel("EmissiveSphere2", "Resources/Models/sphere.obj", sphereLoadParams);
	emissiveSphere2->SetPosition(glm::vec3(-20.0f, 30.0f, -20.0f));
	auto emissiveSphere2Mat = emissiveSphere2->GetMaterial(0);

	emissiveSphere2Mat->SetForceFactor(EMaterialTexture::BaseColor, true);
	emissiveSphere2Mat->SetForceFactor(EMaterialTexture::Emissive, true);
	emissiveSphere2Mat->SetEmissiveFactor(glm::vec3(1.0f, 1.0f, 0.0f));
	emissiveSphere2Mat->SetEmissiveIntensity(1.0f);
	emissiveSphere2Mat->SetBaseColorFactor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

	auto emissiveSphere3 = this->LoadModel("EmissiveSphere3", "Resources/Models/sphere.obj", sphereLoadParams);
	emissiveSphere3->SetPosition(glm::vec3(-40.0f, 30.0f, -20.0f));
	auto emissiveSphere3Mat = emissiveSphere3->GetMaterial(0);

	emissiveSphere3Mat->SetForceFactor(EMaterialTexture::BaseColor, true);
	emissiveSphere3Mat->SetForceFactor(EMaterialTexture::Emissive, true);
	emissiveSphere3Mat->SetEmissiveFactor(glm::vec3(1.0f, 0.0f, 0.0f));
	emissiveSphere3Mat->SetEmissiveIntensity(1.0f);
	emissiveSphere3Mat->SetBaseColorFactor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

	auto emissiveSphere4 = this->LoadModel("EmissiveSphere4", "Resources/Models/sphere.obj", sphereLoadParams);
	emissiveSphere4->SetPosition(glm::vec3(-60.0f, 30.0f, -20.0f));
	auto emissiveSphere4Mat = emissiveSphere4->GetMaterial(0);

	emissiveSphere4Mat->SetForceFactor(EMaterialTexture::BaseColor, true);
	emissiveSphere4Mat->SetForceFactor(EMaterialTexture::Emissive, true);
	emissiveSphere4Mat->SetEmissiveFactor(glm::vec3(1.0f, 1.0f, 1.0f));
	emissiveSphere4Mat->SetEmissiveIntensity(1.0f);
	emissiveSphere4Mat->SetBaseColorFactor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

	auto emissiveSphere5 = this->LoadModel("EmissiveSphere5", "Resources/Models/sphere.obj", sphereLoadParams);
	emissiveSphere5->SetPosition(glm::vec3(-60.0f, 30.0f, -7.0f));
	auto emissiveSphere5Mat = emissiveSphere5->GetMaterial(0);

	emissiveSphere5Mat->SetForceFactor(EMaterialTexture::BaseColor, true);
	emissiveSphere5Mat->SetForceFactor(EMaterialTexture::Emissive, true);
	emissiveSphere5Mat->SetEmissiveFactor(glm::vec3(1.0f, 1.0f, 1.0f));
	emissiveSphere5Mat->SetEmissiveIntensity(1.0f);
	emissiveSphere5Mat->SetBaseColorFactor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

	auto emissiveSphere6 = this->LoadModel("EmissiveSphere4", "Resources/Models/sphere.obj", sphereLoadParams);
	emissiveSphere6->SetPosition(glm::vec3(-60.0f, 30.0f, 10.0f));
	auto emissiveSphere6Mat = emissiveSphere6->GetMaterial(0);

	emissiveSphere6Mat->SetForceFactor(EMaterialTexture::BaseColor, true);
	emissiveSphere6Mat->SetForceFactor(EMaterialTexture::Emissive, true);
	emissiveSphere6Mat->SetEmissiveFactor(glm::vec3(1.0f, 1.0f, 1.0f));
	emissiveSphere6Mat->SetEmissiveIntensity(1.0f);
	emissiveSphere6Mat->SetBaseColorFactor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

	Model* metallicSphere = this->LoadModel("MetallicSphere", "Resources/Models/sphere.obj", sphereLoadParams);
	metallicSphere->SetPosition(glm::vec3(-7.0f, 2.0f, -5.5f));
	auto metallicSphereMat = metallicSphere->GetMaterial(0);
	metallicSphereMat->SetForceFactor(EMaterialTexture::BaseColor, true);
	metallicSphereMat->SetForceFactor(EMaterialTexture::MetallicRoughness, true);
	metallicSphereMat->SetBaseColorFactor(glm::vec4(1.0f));
	metallicSphereMat->SetMetallicFactor(1.0f);
	metallicSphereMat->SetRoughnessFactor(0.0f);

	auto refractiveBunny = this->LoadModel("RefractiveBunny", "Resources/Models/bunny.obj", sphereLoadParams);
	refractiveBunny->SetPosition(glm::vec3(-21.0f, 0.0f, -7.5f));
	refractiveBunny->SetScale(glm::vec3(1.5f));
	refractiveBunny->bCastShadow = false;
	auto refractiveBunnyMat = refractiveBunny->GetMaterial(0);
	refractiveBunnyMat->SetForceFactor(EMaterialTexture::BaseColor, true);
	refractiveBunnyMat->SetBaseColorFactor(glm::vec4(0.0f));
	refractiveBunnyMat->bRefract = true;
	refractiveBunnyMat->IOR = 1.2f;

	m_quad = this->CreatePlane("EmissivePlane");
	m_quad->bDoubleSided = true;
	m_quad->SetPosition(glm::vec3(45.0f, 5.5f, 1.5f));
	m_quad->SetRotation(glm::rotate(glm::quat(), glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
	m_quad->SetScale(glm::vec3(7.5f, 4.5f, 1.0f));
	auto quadMat = m_quad->GetMaterial(0);
	quadMat->SetForceFactor(EMaterialTexture::BaseColor, true);
	quadMat->SetBaseColorFactor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

	std::filesystem::path animeEmissiveTexture = "Resources/Textures/fd376fc20378ec4fe5f372a82b6d55ccd9dbec2e.png";
	if (std::filesystem::exists(animeEmissiveTexture))
	{
		quadMat->SetEmissive(new Texture2D("Resources/Textures/fd376fc20378ec4fe5f372a82b6d55ccd9dbec2e.png"));
	}
	else
	{
		quadMat->SetForceFactor(EMaterialTexture::Emissive, true);
		quadMat->SetEmissiveFactor(glm::vec3(1.0f));
	}
	quadMat->SetEmissiveIntensity(2.0f);

	m_mainLight = this->CreateLight("Main");
	// Direct Sunlight
	m_mainLight->SetIntensity(glm::vec3{ 3.0f });
	// Clear Blue Sky
	//m_mainLight->SetIntensity(5.0f * glm::vec3{0.25098f, 0.611765f, 1.0f});
	// 100W Tungsten
	//m_mainLight->SetIntensity(5.0f * glm::vec3(1.0f, 0.945f, 0.6667f));
	this->UpdateLightRotation();

	m_cam = this->GetMainCamera();
	m_cam->SetPosition(glm::vec3(-40.0f, 2.5f, 0.0f));

	m_camPath.Paths.push_back(Path{
	.p0 = glm::vec3(-40.0f, 3.5f, 0.0f),
	.p1 = glm::vec3(-40.0f, 5.0f, 0.0f),
	.r0 = glm::rotate(glm::quat(), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
	.r1 = glm::rotate(glm::quat(), glm::radians(-270.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
	.Speed = 0.1f
		});

	m_camPath.Paths.push_back(Path{
		.p0 = glm::vec3(-40.0f, 5.0f, 0.0f),
		.p1 = glm::vec3(30.0f, 5.0f, 0.0f),
		.r0 = glm::rotate(glm::quat(), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
		.r1 = glm::rotate(glm::quat(), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
		.Speed = 0.15f
		});

	m_camPath.Paths.push_back(Path{
		.p0 = glm::vec3(30.0f, 5.0f, 0.0f),
		.p1 = glm::vec3(10.0f, 30.0f, 0.0f),
		.r0 = glm::rotate(glm::quat(), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
		.r1 = glm::rotate(glm::quat(), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
		.Speed = 0.15f
		});

	m_camPath.Paths.push_back(Path{
		.p0 = glm::vec3(10.0f, 30.0f, 0.0f),
		.p1 = glm::vec3(-20.0f, 30.0f, 0.0f),
		.r0 = glm::rotate(glm::quat(), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
		.r1 = glm::rotate(glm::quat(), glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
		.Speed = 0.2f
		});

	m_camPath.Paths.push_back(Path{
	.p0 = glm::vec3(-20.0f, 30.0f, 0.0f),
	.p1 = glm::vec3(25.0f, 65.0f, 0.0f),
	.r0 = glm::rotate(glm::quat(), glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
	.r1 = glm::rotate(glm::quat(), glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(glm::quat(), glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
	.Speed = 0.2f
		});

	m_camPath.bResetOnFinish = true;
}

void SponzaScene::Update(float dt)
{
	if (m_bLightRotate)
	{
		m_lightElapsedTime += dt * 0.08f;
		constexpr float altitudeAngle = glm::radians(20.0f);
		const float azimuthAngle = m_lightElapsedTime * 2.0f * PI;
		glm::vec3 lightPos = glm::vec3(std::sin(altitudeAngle) * std::cos(azimuthAngle), std::cos(altitudeAngle), std::sin(altitudeAngle) * std::sin(azimuthAngle));
		m_mainLight->SetPosition(lightPos);
		m_sphere->SetPosition(lightPos * 5.0f + glm::vec3(0.0f, 5.0f, 0.0f));
	}
	else
	{
		if (m_lightRotationDirX != 0.0f || m_lightRotationDirY != 0.0f)
		{
			m_lightRotationX += m_lightRotationDirX * m_lightRotationSpeed * dt;
			m_lightRotationY += m_lightRotationDirY * m_lightRotationSpeed * dt;
			this->UpdateLightRotation();
		}
	}

	if (m_bEnableCamPath && m_cam != nullptr)
	{
		const glm::vec3 forward = glm::vec3(0.0f, 0.0f, 1.0f);

		const auto camPos = m_camPath.GetCurrentPosition();
		const auto camRot = m_camPath.GetCurrentRotation();
		m_cam->SetPosition(camPos);
		m_cam->SetLookAt(camPos + (glm::rotate(camRot, forward)));

		m_camPath.Update(dt);
	}
}

void SponzaScene::KeyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		default:
			break;

		case GLFW_KEY_F1:
			m_quad->SetPosition(m_quad->GetPosition() - glm::vec3(10.0f, 0.0f, 0.0f));
			break;

		case GLFW_KEY_F2:
			m_quad->SetPosition(m_quad->GetPosition() + glm::vec3(10.0f, 0.0f, 0.0f));
			break;

		case GLFW_KEY_R:
			this->m_cam->SetPosition(glm::vec3(-40.0f, 3.5f, 0.0f));
			m_lightRotationX = 105.0f;
			m_lightRotationY = 0.0f;
			this->UpdateLightRotation();
			m_bEnableCamPath = false;
			break;

		case GLFW_KEY_RIGHT:
			m_lightRotationDirX = -1.0f;
			break;

		case GLFW_KEY_LEFT:
			m_lightRotationDirX = 1.0f;
			break;

		case GLFW_KEY_DOWN:
			m_lightRotationDirY = -1.0f;
			break;

		case GLFW_KEY_UP:
			m_lightRotationDirY = 1.0f;
			break;

		case GLFW_KEY_E:
			m_sphere->SetActive(!m_sphere->IsActivated());
			break;

		case GLFW_KEY_MINUS:
			m_floorRoughness -= 0.1f;
			m_floorRoughness = std::max(0.0f, m_floorRoughness);
			m_floorMaterial->SetRoughnessFactor(m_floorRoughness);
			break;

		case GLFW_KEY_EQUAL:
			m_floorRoughness += 0.1f;
			m_floorRoughness = std::min(1.0f, m_floorRoughness);
			m_floorMaterial->SetRoughnessFactor(m_floorRoughness);
			break;

		case GLFW_KEY_C:
			m_bEnableCamPath = !m_bEnableCamPath;
			if (m_bEnableCamPath)
			{
				m_camPath.ResetProgress();
			}
			break;

		case GLFW_KEY_L:
			m_bLightRotate = !m_bLightRotate;
			m_mainLight->bUseRotation = !m_bLightRotate;
			m_sphere->SetActive(m_bLightRotate);
			break;

		}
	}
	else if (action == GLFW_RELEASE)
	{
		switch (key)
		{
		case GLFW_KEY_RIGHT:
			m_lightRotationDirX = 0.0f;
			break;

		case GLFW_KEY_LEFT:
			m_lightRotationDirX = 0.0f;
			break;

		case GLFW_KEY_DOWN:
			m_lightRotationDirY = 0.0f;
			break;

		case GLFW_KEY_UP:
			m_lightRotationDirY = 0.0f;
			break;
		}
	}
}

void SponzaScene::UpdateLightRotation()
{
	const glm::quat lightXRot = glm::rotate(glm::quat(),
		glm::radians(m_lightRotationX), glm::vec3{ 1.0f, 0.0f, 0.0f });

	const glm::quat lightYRot = glm::rotate(glm::quat(),
		glm::radians(m_lightRotationY), glm::vec3{ 0.0f, 1.0f, 0.0f });

	m_mainLight->SetRotation(lightXRot * lightYRot);
}
