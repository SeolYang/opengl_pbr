#include "Renderer.h"
#include "Scene.h"
#include "Mesh.h"
#include "Model.h"
#include "Camera.h"
#include "Viewport.h"
#include "Shader.h"
#include "Light.h"

#include "GL/gl3w.h"

Renderer::Renderer() :
	m_basicShader(nullptr)
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

void Renderer::Render(Scene* scene)
{
	bool isValidRenderRequest = scene != nullptr;
	if (isValidRenderRequest)
	{
		std::vector<Camera*>& cameras = scene->GetCameras();
		std::vector<Model*>& models = scene->GetModels();
		std::vector<Light*>& lights = scene->GetLights();

		Camera* mainCamera = scene->GetMainCamera();

		glm::vec3 clearColor = mainCamera->GetClearColor();
		Clear(glm::vec4{ clearColor, 1.0f });

		for (const auto* camera : cameras)
		{
			if (camera != nullptr && camera->IsActivated())
			{
				Viewport* viewport = camera->GetViewport();
				viewport->Bind();

				glm::mat4 viewMat = camera->GetViewMatrix();
				glm::mat4 projMat = camera->GetProjMatrix();

				m_basicShader->Bind();
				m_basicShader->SetMat4f("viewMatrix", viewMat);
				m_basicShader->SetMat4f("projMatrix", projMat);

				m_basicShader->SetVec3f("camPos", camera->GetPosition());

				// ########### TEST CODE ##############
				auto numOfLights = (lights.size() <= MaximumLights) ? lights.size() : MaximumLights;
				m_basicShader->SetInt("numOfLights", numOfLights);
				for (size_t idx = 0; idx < numOfLights; ++idx)
				{
					auto indexingStr = "lights[" + std::to_string(idx) + "]";
					m_basicShader->SetVec3f(indexingStr + ".position", lights[idx]->GetPosition());
					m_basicShader->SetVec3f(indexingStr + ".radiance", lights[idx]->GetRadiance());
				}

				for (auto model : models)
				{
					m_basicShader->SetMat4f("worldMatrix", model->GetWorldMatrix());
					if (model != nullptr)
					{
						model->Render(m_basicShader);
					}
				}
			}
		}
	}
}

void Renderer::Clear(const glm::vec4& color, bool clearDepth)
{
	GLbitfield clearBit = GL_COLOR_BUFFER_BIT | 
		(clearDepth ? GL_DEPTH_BUFFER_BIT : 0);
	glClearColor(color.r, color.g, color.b, color.a);
	glClear(clearBit);
}