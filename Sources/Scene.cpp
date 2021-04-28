#include "Scene.h"
#include "Camera.h"
#include "Light.h"
#include "Model.h"

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
	Light* newLight = new Light(name);
	m_lights.push_back(newLight);
	return newLight;
}

Camera* Scene::CreateCamera(const std::string& name)
{
	Camera* newCamera = new Camera(name);
	m_cameras.push_back(newCamera);
	return newCamera;
}

Model* Scene::LoadModel(const std::string& name, const std::string& filePath, const ModelLoadParams& params)
{
	Model* newModel = new Model(name, filePath, params);
	m_models.push_back(newModel);
	return newModel;
}