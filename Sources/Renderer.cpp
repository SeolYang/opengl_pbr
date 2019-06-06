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
	m_model(ELightingModel::CookTorrance),
	m_pbrShader(nullptr),
	m_phongShader(nullptr)
{
}

Renderer::~Renderer()
{
	if (m_pbrShader != nullptr)
	{
		delete m_pbrShader;
		m_pbrShader = nullptr;
	}

	if (m_phongShader != nullptr)
	{
		delete m_phongShader;
		m_phongShader = nullptr;
	}
}

bool Renderer::Init()
{
	m_pbrShader = new Shader(
		"../Resources/Shaders/PBR.vs", 
		"../Resources/Shaders/PBR.fs");


	m_phongShader = new Shader(
		"../Resources/Shaders/Phong.vs",
		"../Resources/Shaders/Phong.fs");

	glEnable(GL_DEPTH_TEST);

	return true;
}

void Renderer::Render(Scene* scene)
{
	bool isValidRenderRequest = scene != nullptr;
	if (isValidRenderRequest)
	{
		Shader* targetShader = nullptr;
		switch (m_model)
		{
		case ELightingModel::CookTorrance:
			targetShader = m_pbrShader;
			break;
		default:
			targetShader = m_phongShader;
			break;
		}

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

				targetShader->Bind();
				targetShader->SetMat4f("viewMatrix", viewMat);
				targetShader->SetMat4f("projMatrix", projMat);

				targetShader->SetVec3f("camPos", camera->GetPosition());

				// ########### TEST CODE ##############
				auto numOfLights = (lights.size() <= MaximumLights) ? lights.size() : MaximumLights;
				targetShader->SetInt("numOfLights", numOfLights);
				for (size_t idx = 0; idx < numOfLights; ++idx)
				{
					auto indexingStr = "lights[" + std::to_string(idx) + "]";
					targetShader->SetVec3f(indexingStr + ".position", lights[idx]->GetPosition());
					targetShader->SetVec3f(indexingStr + ".radiance", lights[idx]->GetRadiance());
				}

				for (auto model : models)
				{
					targetShader->SetMat4f("worldMatrix", model->GetWorldMatrix());
					if (model != nullptr)
					{
						model->Render(targetShader);
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