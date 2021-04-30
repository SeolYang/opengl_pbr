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
#include "ShadowMap.h"

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
	delete m_renderVoxelPass;
	delete m_cube;
	delete m_cubeBack;
	delete m_cubeFront;
	delete m_shadowMap;
	delete m_shadowPass;
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

	m_shadowPass = new Shader(
		"Resources/Shaders/ShadowVS.vert",
		"Resources/Shaders/ShadowFS.frag");
	m_shadowMap = new ShadowMap(ShadowMapRes, ShadowMapRes);

	m_voxelVolume = new Texture3D(
		std::vector<GLfloat>(4 * VoxelUnitSize * VoxelUnitSize * VoxelUnitSize, 0.0f),
		VoxelUnitSize, VoxelUnitSize, VoxelUnitSize);

	const float gridSize = static_cast<float>(VoxelGridWorldSize);
	const glm::mat4 projMat = glm::ortho(-gridSize * 0.5f, gridSize * 0.5f, -gridSize * 0.5f, gridSize * 0.5f, gridSize * 0.5f, gridSize * 1.5f);
	m_projX = projMat * glm::lookAt(glm::vec3(gridSize, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	m_projY = projMat * glm::lookAt(glm::vec3(0.0f, gridSize, 0.0f), glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	m_projZ = projMat * glm::lookAt(glm::vec3(0.0f, 0.0f, gridSize), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

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

	m_renderVoxelPass = new Shader(
		"Resources/Shaders/RenderVoxel.vert",
		"Resources/Shaders/RenderVoxel.geom",
		"Resources/Shaders/RenderVoxel.frag");

	glGenVertexArrays(1, &m_texture3DVAO);

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
	Shadow(scene);
	switch(m_renderMode)
	{
	case ERenderMode::VCT:
		Voxelize(scene);
		break;

	case ERenderMode::VoxelVisualization:
		Voxelize(scene);
		//VisualizeVoxel(scene);
		RenderVoxel(scene);
		break;

	case ERenderMode::Deferred:
	default:
		DeferredRender(scene);
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
			m_lightingPass->Bind();
			m_gBuffer->BindTextures();
			m_shadowMap->BindAsTexture(5);
			m_lightingPass->SetInt("shadowMap", 5);
			m_lightingPass->SetVec3f("camPos", camera->GetPosition());
			m_lightingPass->SetMat4f("shadowViewMat", m_shadowViewMat);
			m_lightingPass->SetMat4f("shadowProjMat", m_shadowProjMat);
			if (!lights.empty())
			{
				m_lightingPass->SetVec3f("light.Direction", lights[0]->Forward());
				m_lightingPass->SetVec3f("light.Intensity", lights[0]->GetIntensity());
			}

			glDisable(GL_DEPTH_TEST);
			glBindVertexArray(m_quadVAO);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glBindVertexArray(0);
			m_gBuffer->UnbindTextures();
		}
	}
}

void Renderer::Shadow(const Scene* scene)
{
	if (scene->IsSceneDirty())
	{
		auto lights = scene->GetLights();
		if (!lights.empty())
		{
			m_shadowPass->Bind();
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glFrontFace(GL_CCW);

			m_shadowMap->Bind();
			glViewport(0, 0, ShadowMapRes, ShadowMapRes);
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			m_shadowViewMat = glm::lookAt(-lights[0]->Forward(), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			m_shadowProjMat = glm::ortho<float>(-120.0f, 120.0f, -120.0f, 120.0f, -500.0f, 500.0f);
			m_shadowPass->SetMat4f("viewMatrix", m_shadowViewMat);
			m_shadowPass->SetMat4f("projMatrix", m_shadowProjMat);

			auto models = scene->GetModels();
			for (auto model : models)
			{
				m_shadowPass->SetMat4f("worldMatrix", model->GetWorldMatrix());
				model->Render(m_shadowPass);
			}

			m_shadowMap->Unbind();
			glViewport(0, 0, m_winWidth, m_winHeight);
		}
	}
}

void Renderer::Voxelize(const Scene* scene)
{
	if (scene != nullptr)
	{
		if (m_bAlwaysComputeVoxel || scene->IsSceneDirty())
		{
			const std::vector<Light*> lights = scene->GetLights();
			if (!lights.empty())
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

				// Vertex Shader Uniforms
				m_voxelizePass->SetMat4f("shadowViewMat", m_shadowViewMat);
				m_voxelizePass->SetMat4f("shadowProjMat", m_shadowProjMat);

				// Geometry Shader Uniforms
				m_voxelizePass->SetMat4f("projXAxis", m_projX);
				m_voxelizePass->SetMat4f("projYAxis", m_projY);
				m_voxelizePass->SetMat4f("projZAxis", m_projZ);

				// Fragment Shader Uniforms
				m_voxelizePass->SetVec3f("light.Direction", lights[0]->Forward());
				m_voxelizePass->SetVec3f("light.Intensity", lights[0]->GetIntensity());
				m_voxelizePass->SetInt("shadowMap", 5);
				m_shadowMap->BindAsTexture(5);
				m_voxelVolume->Bind(6);
				glBindImageTexture(0, m_voxelVolume->GetID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);
				//glBindImageTexture(0, m_voxelVolume->GetID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_R32UI);

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
				m_shadowMap->UnbindAsTexture(5);
				m_voxelVolume->Unbind(6);
				glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
				glDisable(GL_CONSERVATIVE_RASTERIZATION_NV);
			}
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

void Renderer::RenderVoxel(const Scene* scene)
{
	if (scene != nullptr)
	{
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		//glDisable(GL_DEPTH_TEST);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, m_winWidth, m_winHeight);

		m_renderVoxelPass->Bind();
		m_renderVoxelPass->SetInt("volumeDim", VoxelUnitSize);
		m_voxelVolume->Bind(0);
		m_renderVoxelPass->SetInt("voxelVolume", 0);

		Camera* camera = scene->GetMainCamera();
		glm::mat4 modelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(VoxelSize));
		glm::mat4 viewMatrix = camera->GetViewMatrix();
		glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;
		glm::mat4 projMatrix = camera->GetProjMatrix();
		m_renderVoxelPass->SetMat4f("modelViewMatrix", modelViewMatrix);
		m_renderVoxelPass->SetMat4f("projectionMatrix", projMatrix);

		glBindVertexArray(m_texture3DVAO);
		glDrawArrays(GL_POINTS, 0, VoxelNum);

		glBindVertexArray(0);
		m_voxelVolume->Unbind(0);
	}
}

void Renderer::Clear(const glm::vec4& color, bool clearDepth)
{
	GLbitfield clearBit = GL_COLOR_BUFFER_BIT | (clearDepth ? GL_DEPTH_BUFFER_BIT : 0);
	glClearColor(color.r, color.g, color.b, color.a);
	glClear(clearBit);
}
