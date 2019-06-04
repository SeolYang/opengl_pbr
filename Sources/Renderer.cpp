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
	m_basicShader = new Shader(
		"../Resources/Shaders/PBR.vs", 
		"../Resources/Shaders/PBR.fs");

	glEnable(GL_DEPTH_TEST);

	return true;
}

void Renderer::Render(Scene* scene, Viewport* viewport)
{
	bool isValidRenderRequest = scene != nullptr && viewport != nullptr;
	if (isValidRenderRequest)
	{
		m_targetScene = scene;
		m_targetCamera = scene->GetMainCamera();
		m_targetViewport = viewport;

		std::vector<Model*>& models = scene->GetModels();
		std::vector<Light*>& lights = scene->GetLights();

		m_targetViewport->Bind();

		glm::mat4 viewMat = m_targetCamera->GetViewMatrix();
		glm::mat4 projMat = m_targetCamera->GetProjMatrix(
			(float)m_targetViewport->GetWidth(),
			(float)m_targetViewport->GetHeight());

		m_basicShader->Bind();
		m_basicShader->SetMat4f("viewMatrix", viewMat);
		m_basicShader->SetMat4f("projMatrix", projMat);

		m_basicShader->SetVec3f("camPos", m_targetCamera->GetPosition());

		// ########### TEST CODE ##############
		m_basicShader->SetVec3f("lightPos", glm::vec3{ 2.0f, 3.0f, -0.5f });

		glm::vec3 clearColor = m_targetCamera->GetClearColor();
		glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (auto model : models)
		{
			m_basicShader->SetMat4f("worldMatrix", model->GetWorldMatrix());
			if (model != nullptr)
			{
				model->Render(m_basicShader);
			}
		}

		m_targetScene = nullptr;
		m_targetViewport = nullptr;
		m_targetCamera = nullptr;
	}
}