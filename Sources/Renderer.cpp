#include "Renderer.h"
#include "Scene.h"
#include "Mesh.h"
#include "Model.h"
#include "Camera.h"
#include "Viewport.h"
#include "Shader.h"
#include "Light.h"
#include "GBuffer.h"

#include "Rendering.h"

Renderer::Renderer() :
	m_pbrShader(nullptr),
	m_gBuffer(nullptr),
	m_geometryPass(nullptr),
	m_lightingPass(nullptr),
	m_quadVAO(0),
	m_quadVBO(0),
	m_winWidth(0),
	m_winHeight(0)
{
}

Renderer::~Renderer()
{
	if (m_pbrShader != nullptr)
	{
		delete m_pbrShader;
		m_pbrShader = nullptr;
	}

	if (m_gBuffer != nullptr)
	{
		delete m_gBuffer;
		m_gBuffer = nullptr;
	}

	if (m_geometryPass != nullptr)
	{
		delete m_geometryPass;
		m_geometryPass = nullptr;
	}

	if (m_lightingPass != nullptr)
	{
		delete m_lightingPass;
		m_lightingPass = nullptr;
	}
}

bool Renderer::Init(unsigned int width, unsigned int height)
{
	m_winWidth = width;
	m_winHeight = height;

	m_pbrShader = new Shader(
		"../Resources/Shaders/PBR.vs", 
		"../Resources/Shaders/PBR.fs");

	m_gBuffer = new GBuffer(width, height);
	if (!m_gBuffer->Init())
	{
		return false;
	}

	m_geometryPass = new Shader(
		"../Resources/Shaders/GeometryPass.vs",
		"../Resources/Shaders/GeometryPass.fs");

	m_lightingPass = new Shader(
		"../Resources/Shaders/LightingPass.vs",
		"../Resources/Shaders/LightingPass.fs");
	m_lightingPass->Bind();
	m_lightingPass->SetInt("positionBuffer", 0);
	m_lightingPass->SetInt("normalBuffer", 1);
	m_lightingPass->SetInt("albedoBuffer", 2);
	m_lightingPass->SetInt("metallicRoughnessBuffer", 3);
	m_lightingPass->SetInt("emissiveAOBuffer", 4);

	float quadVertices[] = {
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	};

	glGenVertexArrays(1, &m_quadVAO);
	glGenBuffers(1, &m_quadVBO);
	glBindVertexArray(m_quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));

	glEnable(GL_DEPTH_TEST);

	return true;
}

void Renderer::ForwardRender(Scene* scene)
{
	bool isValidRenderRequest = scene != nullptr;
	if (isValidRenderRequest)
	{
		Shader* targetShader = m_pbrShader;

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

void Renderer::DeferredRender(Scene* scene)
{
	bool isValidRenderRequest = scene != nullptr;
	if (isValidRenderRequest)
	{
		std::vector<Camera*>& cameras = scene->GetCameras();
		std::vector<Model*>& models = scene->GetModels();
		std::vector<Light*>& lights = scene->GetLights();

		// @TODO: Impl render to multiple render targets(textures) which owned by cameras
		Camera* camera = scene->GetMainCamera();
		Clear(glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f });
		if (camera != nullptr && camera->IsActivated())
		{
			m_gBuffer->BindFrameBuffer();

			glm::vec3 clearColor = camera->GetClearColor();
			Clear(glm::vec4{ clearColor, 1.0f });

			Viewport* viewport = camera->GetViewport();
			glViewport(0, 0, viewport->GetWidth(), viewport->GetHeight());

			glm::mat4 viewMat = camera->GetViewMatrix();
			glm::mat4 projMat = camera->GetProjMatrix();

			m_geometryPass->Bind();
			m_geometryPass->SetMat4f("viewMatrix", viewMat);
			m_geometryPass->SetMat4f("projMatrix", projMat);

			for (auto model : models)
			{
				m_geometryPass->SetMat4f("worldMatrix", model->GetWorldMatrix());
				if (model != nullptr)
				{
					model->Render(m_geometryPass);
				}
			}
			m_gBuffer->UnbindFrameBuffer();

			// ########### TEST CODE ##############
			glViewport(0, 0, m_winWidth, m_winHeight);
			auto numOfLights = (lights.size() <= MaximumLights) ? lights.size() : MaximumLights;
			m_gBuffer->BindTextures();
			m_lightingPass->Bind();
			m_lightingPass->SetVec3f("camPos", camera->GetPosition());
			m_lightingPass->SetInt("numOfLights", numOfLights);
			for (size_t idx = 0; idx < numOfLights; ++idx)
			{
				auto indexingStr = "lights[" + std::to_string(idx) + "]";
				m_lightingPass->SetVec3f(indexingStr + ".position", lights[idx]->GetPosition());
				m_lightingPass->SetVec3f(indexingStr + ".radiance", lights[idx]->GetRadiance());
			}

			glBindVertexArray(m_quadVAO);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glBindVertexArray(0);
		}

		for (const auto* camera : cameras)
		{
			
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