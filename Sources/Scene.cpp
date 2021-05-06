#include "Scene.h"
#include "Camera.h"
#include "Light.h"
#include "Model.h"
#include "Plane.h"

#include <iostream>

Scene::Scene() :
	m_mainCameraIdx(0)
{
	this->CreateCamera("MainCamera");
}

Scene::~Scene()
{
	for (auto* camera : m_cameras)
	{
		if (camera != nullptr)
		{
			delete camera;
		}
	}

	for (auto* light : m_lights)
	{
		if (light != nullptr)
		{
			delete light;
		}
	}

	for (auto* model : m_models)
	{
		if (model != nullptr)
		{
			delete model;
		}
	}

	m_cameras.clear();
	m_lights.clear();
	m_models.clear();
}

Light* Scene::CreateLight(const std::string& name)
{
	m_bIsDirty = true;
	Light* newLight = new Light(name);
	m_lights.push_back(newLight);
	return newLight;
}

Camera* Scene::CreateCamera(const std::string& name)
{
	m_bIsDirty = true;
	Camera* newCamera = new Camera(name);
	m_cameras.push_back(newCamera);
	return newCamera;
}

Model* Scene::LoadModel(const std::string& name, const std::string& filePath, const ModelLoadParams& params)
{
	m_bIsDirty = true;
	Model* newModel = new Model(name, filePath, params);
	m_models.push_back(newModel);
	return newModel;
}

Model* Scene::CreatePlane(const std::string& name)
{
	m_bIsDirty = true;
	Model* newModel = new Plane(name);
	m_models.push_back(newModel);
	return newModel;
}

bool Scene::IsSceneDirty(bool bIncludeCam) const
{
	if (!m_bIsDirty)
	{
		for (auto model : m_models)
		{
		   if (model->IsDirty())
		   {
				return true;
		   }
		}

		for (auto light : m_lights)
		{
		   if (light->IsDirty())
		   {
				return true;
		   }
		}

		if (bIncludeCam)
		{
			for (auto cam : m_cameras)
			{
				if (cam->IsDirty())
				{
					return true;
				}
			}
		}

		return false;
	}

	return true;
}

void Scene::ResolveDirty(bool bIncludeCam)
{
	for (auto model : m_models)
	{
		model->ResolveDirty();
	}

	for (auto light : m_lights)
	{
		light->ResolveDirty();
	}

	if (bIncludeCam)
	{
		for (auto cam : m_cameras)
		{
			cam->ResolveDirty();
		}
	}

	m_bIsDirty = false;
}
