#include "CornellBoxScene.h"
#include "Model.h"
#include "Camera.h"
#include "Material.h"
#include "Light.h"
#include "Texture2D.h"

#include <iostream>
#include <cmath>
#include <random>
#include <algorithm>
#include <filesystem>
#include <glm/gtx/quaternion.hpp>
#include <glm/glm.hpp>

void CornellBoxScene::Construct()
{
	constexpr float scale = 10.0f;
	Model* backPlane = CreatePlane("Back");
	{
		backPlane->SetPosition(glm::vec3(0.0f, 0.0f, -scale));
		backPlane->SetScale(glm::vec3(scale));

		Material* backPlaneMat = backPlane->GetMaterial(0);
		backPlaneMat->SetForceFactor(EMaterialTexture::BaseColor, true);
		backPlaneMat->SetBaseColorFactor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		backPlaneMat->SetForceFactor(EMaterialTexture::Emissive, true);
		backPlaneMat->SetEmissiveFactor(glm::vec3(0.0f, 0.0f, 0.0f));
		backPlaneMat->SetForceFactor(EMaterialTexture::MetallicRoughness, true);
		backPlaneMat->SetMetallicFactor(0.0f);
		backPlaneMat->SetRoughnessFactor(1.0f);
	}

	Model* leftPlane = CreatePlane("Left");
	{
		leftPlane->SetPosition(glm::vec3(-scale, 0.0f, 0.0f));
		leftPlane->SetScale(glm::vec3(scale));
		leftPlane->SetRotation(glm::rotate(
			glm::quat(),
			glm::radians(90.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)));

		Material* leftPlaneMat = leftPlane->GetMaterial(0);
		leftPlaneMat->SetForceFactor(EMaterialTexture::BaseColor, true);
		leftPlaneMat->SetBaseColorFactor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		leftPlaneMat->SetForceFactor(EMaterialTexture::Emissive, true);
		leftPlaneMat->SetEmissiveFactor(glm::vec3(0.0f, 0.0f, 0.0f));
		leftPlaneMat->SetForceFactor(EMaterialTexture::MetallicRoughness, true);
		leftPlaneMat->SetMetallicFactor(0.0f);
		leftPlaneMat->SetRoughnessFactor(1.0f);
	}

	Model* rightPlane = CreatePlane("Right");
	{
		rightPlane->SetPosition(glm::vec3(scale, 0.0f, 0.0f));
		rightPlane->SetScale(glm::vec3(scale));
		rightPlane->SetRotation(glm::rotate(
			glm::quat(),
			glm::radians(-90.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)));

		Material* rightPlaneMat = rightPlane->GetMaterial(0);
		rightPlaneMat->SetForceFactor(EMaterialTexture::BaseColor, true);
		rightPlaneMat->SetBaseColorFactor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		rightPlaneMat->SetForceFactor(EMaterialTexture::Emissive, true);
		rightPlaneMat->SetEmissiveFactor(glm::vec3(0.0f, 0.0f, 0.0f));
		rightPlaneMat->SetForceFactor(EMaterialTexture::MetallicRoughness, true);
		rightPlaneMat->SetMetallicFactor(0.0f);
		rightPlaneMat->SetRoughnessFactor(1.0f);
	}

	Model* topPlane = CreatePlane("Top");
	{
		topPlane->SetPosition(glm::vec3(0.0f, scale, 0.0f));
		topPlane->SetScale(glm::vec3(scale));
		topPlane->SetRotation(glm::rotate(
			glm::quat(),
			glm::radians(90.0f),
			glm::vec3(1.0f, 0.0f, 0.0f)));

		Material* topPlaneMat = topPlane->GetMaterial(0);
		topPlaneMat->SetForceFactor(EMaterialTexture::BaseColor, true);
		topPlaneMat->SetBaseColorFactor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		topPlaneMat->SetForceFactor(EMaterialTexture::Emissive, true);
		topPlaneMat->SetEmissiveFactor(glm::vec3(0.0f, 0.0f, 0.0f));
		topPlaneMat->SetForceFactor(EMaterialTexture::MetallicRoughness, true);
		topPlaneMat->SetMetallicFactor(0.0f);
		topPlaneMat->SetRoughnessFactor(1.0f);
	}


	Model* bottomPlane = CreatePlane("Bottom");
	{
		bottomPlane->SetPosition(glm::vec3(0.0f, -scale, 0.0f));
		bottomPlane->SetScale(glm::vec3(scale));
		bottomPlane->SetRotation(glm::rotate(
			glm::quat(),
			glm::radians(-90.0f),
			glm::vec3(1.0f, 0.0f, 0.0f)));

		Material* bottomPlaneMat = bottomPlane->GetMaterial(0);
		bottomPlaneMat->SetForceFactor(EMaterialTexture::BaseColor, true);
		bottomPlaneMat->SetBaseColorFactor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		bottomPlaneMat->SetForceFactor(EMaterialTexture::Emissive, true);
		bottomPlaneMat->SetEmissiveFactor(glm::vec3(0.0f, 0.0f, 0.0f));
		bottomPlaneMat->SetForceFactor(EMaterialTexture::MetallicRoughness, true);
		bottomPlaneMat->SetMetallicFactor(0.0f);
		bottomPlaneMat->SetRoughnessFactor(1.0f);
	}

	const ModelLoadParams sphereLoadParams{
	.CalcTangentSpace = true,
	.ConvertToLeftHanded = false,
	.GenSmoothNormals = true,
	.GenUVs = true,
	.PreTransformVertices = true,
	.Triangulate = true
	};

	auto areaLight = CreatePlane("AreaLight");
	{
		areaLight->SetPosition(glm::vec3(0.0f, scale - 0.01f, 0.0f));
		areaLight->SetScale(glm::vec3(scale/4.0f));
		areaLight->SetRotation(glm::rotate(
			glm::quat(),
			glm::radians(90.0f),
			glm::vec3(1.0f, 0.0f, 0.0f)));
		areaLight->bCastShadow = false;
		auto material = areaLight->GetMaterial(0);
		material->SetForceFactor(EMaterialTexture::Emissive, true);
		material->SetForceFactor(EMaterialTexture::BaseColor, true);
		material->SetBaseColorFactor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		material->SetEmissiveFactor(glm::vec3(3.0f));
		material->SetForceFactor(EMaterialTexture::MetallicRoughness, true);
		material->SetMetallicFactor(0.0f);
		material->SetRoughnessFactor(0.0f);
	}

	m_mainLight = this->CreateLight("Sun");
	{
		m_mainLight->SetIntensity(glm::vec3{ 0.0f });
		m_mainLight->bUseRotation = false;
		m_mainLight->SetPosition(glm::vec3(0.0f, 0.0f, 1.0f));
		this->UpdateLightRotation();
	}

	auto mainCamera = this->GetMainCamera();
	{
		mainCamera->SetPosition(glm::vec3(0.0f, 0.0f, 30.0f));
	}
}

void CornellBoxScene::Update(float dt)
{
	if (m_lightRotationDirX != 0.0f || m_lightRotationDirY != 0.0f)
	{
		m_lightRotationX += m_lightRotationDirX * m_lightRotationSpeed * dt;
		m_lightRotationY += m_lightRotationDirY * m_lightRotationSpeed * dt;
		this->UpdateLightRotation();
	}
}

void CornellBoxScene::KeyCallback(struct GLFWwindow* window, int key, int scanCode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
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

void CornellBoxScene::UpdateLightRotation()
{
	const glm::quat lightXRot = glm::rotate(glm::quat(),
		glm::radians(m_lightRotationX), glm::vec3{ 1.0f, 0.0f, 0.0f });

	const glm::quat lightYRot = glm::rotate(glm::quat(),
		glm::radians(m_lightRotationY), glm::vec3{ 0.0f, 1.0f, 0.0f });

	m_mainLight->SetRotation(lightXRot * lightYRot);
}
