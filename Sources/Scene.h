#pragma once
#include <vector>
#include <string>

#include "glm/vec3.hpp"

class Model;
class Camera;
class Light;
struct GLFWwindow;
struct ModelLoadParams;
class Scene
{
public:
	Scene();
	virtual ~Scene();

	std::vector<Camera*>& GetCameras() { return m_cameras; }
	std::vector<Light*>& GetLights() { return m_lights; }
	std::vector<Model*>& GetModels() { return m_models; }

	std::vector<Camera*> GetCameras() const { return m_cameras; }
	std::vector<Light*> GetLights() const { return m_lights; }
	std::vector<Model*> GetModels() const { return m_models; }

	Light* CreateLight(const std::string& name);
	Camera* CreateCamera(const std::string& name);
	Model* LoadModel(const std::string& name, const std::string& filePath, const ModelLoadParams& params);

	Camera* SetMainCamera(unsigned int idx) { m_mainCameraIdx = idx; }
	Camera* GetMainCamera() const { return m_cameras[m_mainCameraIdx]; }

	bool IsSceneDirty(bool bIncludeCam = false) const;
	void ResolveDirty(bool bIncludeCam = false);

	virtual void Construct() { }
	virtual void Update(float dt) { }
	virtual void KeyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods) {}

private:
	unsigned int				m_mainCameraIdx;
	std::vector<Camera*>		m_cameras;
	std::vector<Light*>		m_lights;
	std::vector<Model*>		m_models;
	bool							m_bIsDirty = true;

};