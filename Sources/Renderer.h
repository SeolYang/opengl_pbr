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
class GBuffer;
class Renderer
{
public:
	Renderer();
	~Renderer();

	bool Init(unsigned int width, unsigned int height);
	void DeferredRender(Scene* scene);

	void Clear(const glm::vec4& color, bool clearDepth = true);

private:
	Shader* m_pbrShader;

	GBuffer*	m_gBuffer;
	Shader*		m_geometryPass;
	Shader*		m_lightingPass;

	unsigned int m_quadVAO;
	unsigned int m_quadVBO;

	unsigned int m_winWidth;
	unsigned int m_winHeight;

};