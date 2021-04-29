#include "Renderer.h"
#include "Scene.h"
#include "Mesh.h"
#include "Model.h"
#include "Camera.h"
#include "Viewport.h"
#include "Shader.h"
#include "Light.h"
#include "GBuffer.h"
#include "Texture2D.h"
#include "Texture3D.h"
#include "FBO.h"

Renderer::~Renderer()
{
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

	if (m_voxelVolume != nullptr)
	{
		delete m_voxelVolume;
	}

	if (m_voxelizePass != nullptr)
	{
		delete m_voxelizePass;
	}

	delete m_worldPosPass;
	delete m_visualizeVoxelPass;
	delete m_cube;
	delete m_cubeBack;
	delete m_cubeFront;
}

bool Renderer::Init(unsigned int width, unsigned int height)
{
	m_winWidth = width;
	m_winHeight = height;

	m_gBuffer = new GBuffer(width, height);
	if (!m_gBuffer->Init())
	{
		return false;
	}

	m_geometryPass = new Shader(
		"Resources/Shaders/GeometryPass.vs",
		"Resources/Shaders/GeometryPass.fs");

	m_lightingPass = new Shader(
		"Resources/Shaders/LightingPass.vs",
		"Resources/Shaders/LightingPass.fs");
	m_lightingPass->Bind();
	m_lightingPass->SetInt("positionBuffer", 0);
	m_lightingPass->SetInt("normalBuffer", 1);
	m_lightingPass->SetInt("albedoBuffer", 2);
	m_lightingPass->SetInt("metallicRoughnessBuffer", 3);
	m_lightingPass->SetInt("emissiveAOBuffer", 4);

	m_voxelVolume = new Texture3D(
		std::vector<GLfloat>(4 * VoxelUnitSize * VoxelUnitSize * VoxelUnitSize, 0.0f),
		VoxelUnitSize, VoxelUnitSize, VoxelUnitSize);

	//m_voxelVolume = new Texture3D(
	//	std::vector<GLuint>(VoxelUnitSize * VoxelUnitSize * VoxelUnitSize, 0),
	//	VoxelUnitSize, VoxelUnitSize, VoxelUnitSize);

	m_voxelizePass = new Shader(
		"Resources/Shaders/VoxelizationVS.glsl",
		"Resources/Shaders/VoxelizationGS.glsl",
		"Resources/Shaders/VoxelizationFS.glsl");

	m_worldPosPass = new Shader(
		"Resources/Shaders/WorldPosVS.glsl",
		"Resources/Shaders/WorldPosFS.glsl");

	m_visualizeVoxelPass = new Shader(
		"Resources/Shaders/VisualizeVoxelVS.glsl",
		"Resources/Shaders/VisualizeVoxelFS.glsl");

	m_cubeBack = new FBO(m_winWidth, m_winHeight);
	m_cubeFront = new FBO(m_winWidth, m_winHeight);

	ModelLoadParams cubeParams
	{
			.CalcTangentSpace = true,
			.ConvertToLeftHanded = true,
			.GenSmoothNormals = true,
			.GenUVs = true,
			.PreTransformVertices = true,
			.Triangulate = false
	};

	m_cube = new Model("Cube", "Resources/Models/cube.obj", cubeParams);

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
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	//glEnable(GL_MULTISAMPLE);

	return true;
}

void Renderer::Render(const Scene* scene)
{
	switch(m_renderMode)
	{
	case ERenderMode::VCT:
		Voxelize(scene);
		break;

	case ERenderMode::VoxelVisualization:
		Voxelize(scene);
		VisualizeVoxel(scene);
		break;

	case ERenderMode::Deferred:
	default:
		this->DeferredRender(scene);
		break;
	}
}

void Renderer::DeferredRender(const Scene* scene)
{
	if (scene != nullptr)
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);

		const std::vector<Camera*>& cameras = scene->GetCameras();
		const std::vector<Model*>& models = scene->GetModels();
		const std::vector<Light*>& lights = scene->GetLights();

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

			glEnable(GL_DEPTH_TEST);

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
				m_lightingPass->SetVec3f(indexingStr + ".intensity", lights[idx]->GetIntensity());
			}

			glDisable(GL_DEPTH_TEST);
			glBindVertexArray(m_quadVAO);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glBindVertexArray(0);
			m_gBuffer->UnbindTextures();
		}
	}
}

void Renderer::Voxelize(const Scene* scene)
{
	if (scene != nullptr)
	{
		if (m_bAlwaysComputeVoxel || scene->IsSceneDirty())
		{
			glEnable(GL_CONSERVATIVE_RASTERIZATION_NV);
			m_voxelizePass->Bind();

			Camera* camera = scene->GetMainCamera();
			m_voxelizePass->SetVec3f("camPos", camera->GetPosition());

			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			glViewport(0, 0, VoxelUnitSize, VoxelUnitSize);
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
			glDisable(GL_CULL_FACE);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_BLEND);

			const std::vector<Light*> lights = scene->GetLights();
			const auto numOfLights = (lights.size() <= MaximumLights) ? lights.size() : MaximumLights;

			m_voxelVolume->Bind(5);
			glBindImageTexture(0, m_voxelVolume->GetID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);
			//glBindImageTexture(0, m_voxelVolume->GetID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_R32UI);

			m_voxelizePass->SetInt("numOfLights", numOfLights);
			for (size_t idx = 0; idx < numOfLights; ++idx)
			{
				auto indexingStr = "lights[" + std::to_string(idx) + "]";
				m_voxelizePass->SetVec3f(indexingStr + ".Position", lights[idx]->GetPosition());
				m_voxelizePass->SetVec3f(indexingStr + ".Intensity", lights[idx]->GetIntensity());
			}

			const std::vector<Model*> models = scene->GetModels();
			for (auto model : models)
			{
				m_voxelizePass->SetMat4f("worldMatrix", model->GetWorldMatrix());
				if (model != nullptr)
				{
					model->Render(m_voxelizePass);
				}
			}

			glGenerateMipmap(GL_TEXTURE_3D);
			m_voxelVolume->Unbind(5);
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glDisable(GL_CONSERVATIVE_RASTERIZATION_NV);
		}
	}
}

void Renderer::VisualizeVoxel(const Scene* scene)
{
   if (scene != nullptr)
   {
		m_worldPosPass->Bind();

		Camera* camera = scene->GetMainCamera();
		m_worldPosPass->SetMat4f("worldMatrix", glm::mat4());
		m_worldPosPass->SetMat4f("viewMatrix", camera->GetViewMatrix());
		m_worldPosPass->SetMat4f("projMatrix", camera->GetProjMatrix());

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);

		glCullFace(GL_FRONT);
		m_cubeBack->Bind();
		glViewport(0, 0, m_cubeBack->GetWidth(), m_cubeBack->GetHeight());
		m_cubeBack->Clear();
		m_cube->Render(m_worldPosPass);
		m_cubeBack->Unbind();

		glCullFace(GL_BACK);
		m_cubeFront->Bind();
		glViewport(0, 0, m_cubeFront->GetWidth(), m_cubeFront->GetHeight());
		m_cubeFront->Clear();
		m_cube->Render(m_worldPosPass);
		m_cubeFront->Unbind();

		/* Visualization */
		m_visualizeVoxelPass->Bind();
		m_visualizeVoxelPass->SetVec3f("camPos", camera->GetPosition());

		glDisable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		m_cubeBack->BindAsTexture(0);
		m_visualizeVoxelPass->SetInt("cubeBack", 0);
		m_cubeFront->BindAsTexture(1);
		m_visualizeVoxelPass->SetInt("cubeFront", 1);
		m_voxelVolume->Bind(2);
		m_visualizeVoxelPass->SetInt("voxelVolume", 2);

		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, m_winWidth, m_winHeight);
		this->Clear(glm::vec4(camera->GetClearColor(), 1.0f), true);
		glBindVertexArray(m_quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glBindVertexArray(0);
		m_cubeBack->UnbindAsTexture(0);
		m_cubeFront->UnbindAsTexture(1);
		m_voxelVolume->Unbind(2);
   }
}

void Renderer::Clear(const glm::vec4& color, bool clearDepth)
{
	GLbitfield clearBit = GL_COLOR_BUFFER_BIT | (clearDepth ? GL_DEPTH_BUFFER_BIT : 0);
	glClearColor(color.r, color.g, color.b, color.a);
	glClear(clearBit);
}
