#pragma once
#include "glm/vec4.hpp"

constexpr unsigned int MaximumLights = 32;

enum class ELightingModel
{
	Phong,
	BlinnPhong,
	CookTorrance
};

class Scene;
class Mesh;
class Model;
class Light;
class Camera;
class Shader;
class Viewport;
class Renderer
{
public:
	Renderer();
	~Renderer();

	bool Init();
	void Render(Scene* scene);

	void Clear(const glm::vec4& color, bool clearDepth = true);

	void SetLightingModel(ELightingModel model) { m_model = model; }

private:
	ELightingModel m_model;
	Shader* m_pbrShader;
	Shader* m_phongShader;

};