#pragma once
#include "glm/vec4.hpp"

constexpr unsigned int MaximumLights = 32;

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

private:
	Shader* m_basicShader;
	Scene* m_targetScene;

};