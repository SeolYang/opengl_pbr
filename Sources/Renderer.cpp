#include "Renderer.h"
#include "Scene.h"
#include "Mesh.h"
#include "Model.h"
#include "Camera.h"
#include "Viewport.h"
#include "Shader.h"

#include "GL/gl3w.h"

Renderer::Renderer() :
	m_basicShader(nullptr),
	m_targetScene(nullptr),
	m_targetViewport(nullptr),
	m_targetCamera(nullptr)
{
}

Renderer::~Renderer()
{
	if (m_basicShader != nullptr)
	{
		delete m_basicShader;
		m_basicShader = nullptr;
	}
}

bool Renderer::Init()
{
	m_basicShader = new Shader("test.vs", "test.fs");
	return true;
}

void Renderer::Render(Scene* scene, Viewport* viewport)
{
	bool isValidRenderRequest = scene != nullptr && viewport != nullptr;
	if (isValidRenderRequest)
	{
		std::vector<Model*>& models = scene->GetModels();
		std::vector<Light*>& lights = scene->GetLights();

		m_targetScene = scene;
		m_targetViewport = viewport;

		m_targetViewport->Bind();

		glm::mat4 viewMat = m_targetCamera->GetViewMatrix();
		glm::mat4 projMat = m_targetCamera->GetProjMatrix(
			(float)m_targetViewport->GetWidth(),
			(float)m_targetViewport->GetHeight());

		for (auto model : models)
		{
			if (model != nullptr)
			{
				model->Render(m_basicShader);
			}
		}

		m_targetScene = nullptr;
		m_targetViewport = nullptr;
	}
}