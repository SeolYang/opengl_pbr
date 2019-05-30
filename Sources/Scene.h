#pragma once
#include <vector>
#include <string>
#include "glm/vec3.hpp"

class Model;
class Camera;
class Light;
class Scene
{
public:
	Scene();
	~Scene();

	std::vector<Camera*>& GetCameras() { return m_cameras; }
	std::vector<Light*>& GetLights() { return m_lights; }
	std::vector<Model*>& GetModels() { return m_models; }

	std::vector<Camera*> GetCameras() const { return m_cameras; }
	std::vector<Light*> GetLights() const { return m_lights; }
	std::vector<Model*> GetModels() const { return m_models; }

	Light* CreateLight(const std::string& name);
	Camera* CreateCamera(const std::string& name);
	Model* LoadModel(const std::string& filePath, const std::string& name);

	Camera* SetMainCamera(unsigned int idx);
	Camera* SetMainCamera(const std::string& name);

	Camera* GetMainCamera() const { return m_cameras[m_mainCameraIdx]; }

private:
	unsigned int			m_mainCameraIdx;
	std::vector<Camera*>	m_cameras;
	std::vector<Light*>		m_lights;
	std::vector<Model*>		m_models;

};