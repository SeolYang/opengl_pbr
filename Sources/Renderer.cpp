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

	delete m_shadowMap;
	delete m_shadowPass;

	delete m_voxelVolume;
	delete m_voxelizePass;
	delete m_renderVoxelPass;
	delete m_vctPass;
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
		VoxelUnitSize, VoxelUnitSize, VoxelUnitSize, Sampler3D(), std::log2(VoxelUnitSize), true);

	const float gridSize = static_cast<float>(VoxelGridWorldSize);
	const glm::mat4 projMat = glm::ortho(-gridSize * 0.5f, gridSize * 0.5f, -gridSize * 0.5f, gridSize * 0.5f, gridSize * 0.5f, gridSize * 1.5f);
	m_projX = projMat * glm::lookAt(glm::vec3(gridSize, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	m_projY = projMat * glm::lookAt(glm::vec3(0.0f, gridSize, 0.0f), glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	m_projZ = projMat * glm::lookAt(glm::vec3(0.0f, 0.0f, gridSize), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	const auto encodedVoxelVolumeSampler = Sampler3D{
		.MinFilter = GL_NEAREST,
		.MagFilter = GL_NEAREST,
		.WrapS = GL_CLAMP_TO_BORDER,
		.WrapT = GL_CLAMP_TO_BORDER,
		.WrapR = GL_CLAMP_TO_BORDER,
	};

	m_encodedVoxelVolume = new Texture3D(
		std::vector<GLuint>(VoxelUnitSize * VoxelUnitSize * VoxelUnitSize, 0),
		VoxelUnitSize, VoxelUnitSize, VoxelUnitSize, encodedVoxelVolumeSampler, 0, false);

	m_encodedVoxelizePass = new Shader(
		"Resources/Shaders/VoxelizationVS.glsl",
		"Resources/Shaders/VoxelizationGS.glsl",
		"Resources/Shaders/VoxelizationR32UIFS.frag");

	m_voxelizePass = new Shader(
		"Resources/Shaders/VoxelizationVS.glsl",
		"Resources/Shaders/VoxelizationGS.glsl",
		"Resources/Shaders/VoxelizationFS.glsl");

	m_renderVoxelPass = new Shader(
		"Resources/Shaders/RenderVoxel.vert",
		"Resources/Shaders/RenderVoxel.geom",
		"Resources/Shaders/RenderVoxel.frag");

	glGenVertexArrays(1, &m_texture3DVAO);

	m_vctPass = new Shader(
		"Resources/Shaders/VoxelConeTracingVS.vert",
		"Resources/Shaders/VoxelConeTracingFS.frag");

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

	//glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	m_texture3DReductionRGBA = new Shader("Resources/Shaders/Texture3DReductionRGBA8CS.comp");
	m_decodeR32UIToRGBA8 = new Shader("Resources/Shaders/DecodeR32UIToRGBA8CS.comp");

	return true;
}

void Renderer::Render(const Scene* scene)
{
	Shadow(scene);
	//Voxelize(scene);
	EncodedVoxelize(scene);
	switch(m_renderMode)
	{
	case ERenderMode::VCT:
		VoxelConeTracing(scene);
		break;

	case ERenderMode::VoxelVisualization:
		RenderVoxel(scene);
		break;

	case ERenderMode::Deferred:
		DeferredRender(scene);
		break;
	}
}

void Renderer::RenderScene(const Scene* scene, Shader* shader, bool bIsShadowCasting, bool bForceCullFace)
{
	if (const Camera* camera = scene->GetMainCamera(); 
		(camera != nullptr && camera->IsActivated()))
	{
		shader->SetMat4f("viewMatrix", camera->GetViewMatrix());
		shader->SetMat4f("projMatrix", camera->GetProjMatrix());
		shader->SetVec3f("camPos", camera->GetPosition());

		if (auto lights = scene->GetLights(); !lights.empty())
		{
			shader->SetVec3f("light.Direction", lights[0]->LightDirection());
			shader->SetVec3f("light.Intensity", lights[0]->GetIntensity());
		}

		for (auto models = scene->GetModels(); auto model : models)
		{
			if (model != nullptr)
			{
				if (model->IsActivated())
				{
					if (!bIsShadowCasting || model->bCastShadow)
					{
						if (!bForceCullFace)
						{
							if (model->bDoubleSided)
							{
								glDisable(GL_CULL_FACE);
							}
							else
							{
								glEnable(GL_CULL_FACE);
							}
						}

						shader->SetMat4f("worldMatrix", model->GetWorldMatrix());
						model->Render(shader);
					}
				}
			}
		}
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

		// @TODO: Impl render to multiple render targets(textures) which owned by cameras
		Camera* camera = scene->GetMainCamera();
		if (camera != nullptr && camera->IsActivated())
		{
			m_gBuffer->BindFrameBuffer();
			glm::vec3 clearColor = camera->GetClearColor();
			Clear(glm::vec4{ clearColor, 1.0f });

			Viewport* viewport = camera->GetViewport();
			glViewport(0, 0, viewport->GetWidth(), viewport->GetHeight());

			m_geometryPass->Bind();
			glEnable(GL_DEPTH_TEST);
			RenderScene(scene, m_geometryPass);
			m_gBuffer->UnbindFrameBuffer();

			// ########### TEST CODE ##############
			glViewport(0, 0, m_winWidth, m_winHeight);
			m_lightingPass->Bind();
			m_gBuffer->BindTextures();
			m_shadowMap->BindAsTexture(5);
			m_lightingPass->SetInt("shadowMap", 5);
			m_lightingPass->SetVec3f("camPos", camera->GetPosition());
			m_lightingPass->SetMat4f("shadowViewMat", m_shadowViewMat);
			m_lightingPass->SetMat4f("shadowProjMat", m_shadowProjMat);

		   const std::vector<Light*>& lights = scene->GetLights();
			if (!lights.empty())
			{
				m_lightingPass->SetVec3f("light.Direction", lights[0]->LightDirection());
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
	if (scene != nullptr && scene->IsSceneDirty() || m_bFirstShadow)
	{
		m_bFirstShadow = false;
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

			m_shadowViewMat = glm::lookAt(-lights[0]->LightDirection(), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			//m_shadowProjMat = glm::ortho<float>(-120.0f, 120.0f, -120.0f, 120.0f, -500.0f, 500.0f);
			m_shadowProjMat = glm::ortho<float>(-VoxelGridWorldSize, VoxelGridWorldSize, -VoxelGridWorldSize, VoxelGridWorldSize, -500.0f, 500.0f);
			m_shadowPass->SetMat4f("shadowViewMatrix", m_shadowViewMat);
			m_shadowPass->SetMat4f("shadowProjMatrix", m_shadowProjMat);

			RenderScene(scene, m_shadowPass, true);

			m_shadowMap->Unbind();
		}
	}
}

void Renderer::Voxelize(const Scene* scene)
{
	if (scene != nullptr)
	{
		if (m_bFirstVoxelize || scene->IsSceneDirty() || m_bAlwaysComputeVoxel)
		{
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

			m_bFirstVoxelize = false;
			GLfloat volumeClear[4]{ 0.0f, 0.0f, 0.0f, 0.0f };
			m_voxelVolume->Clear(volumeClear);

			m_voxelizePass->Bind();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);

			// Vertex Shader Uniforms
			m_voxelizePass->SetMat4f("shadowViewMat", m_shadowViewMat);
			m_voxelizePass->SetMat4f("shadowProjMat", m_shadowProjMat);

			// Geometry Shader Uniforms
			m_voxelizePass->SetMat4f("projXAxis", m_projX);
			m_voxelizePass->SetMat4f("projYAxis", m_projY);
			m_voxelizePass->SetMat4f("projZAxis", m_projZ);

			// Fragment Shader Uniforms
			m_voxelizePass->SetInt("shadowMap", 5);
			m_shadowMap->BindAsTexture(5);

			glBindImageTexture(0, m_voxelVolume->GetID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);
			//glBindImageTexture(0, m_voxelVolume->GetID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_R32UI);
			glViewport(0, 0, VoxelUnitSize, VoxelUnitSize);
			RenderScene(scene, m_voxelizePass, false, true);

			m_shadowMap->UnbindAsTexture(5);

			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);

			this->GenerateTexture3DMipmap(m_voxelVolume);
		}
	}
}

void Renderer::EncodedVoxelize(const Scene* scene)
{
	if (scene != nullptr)
	{
		m_bNeedVoxelize |= (scene->IsSceneDirty() || m_bAlwaysComputeVoxel);
		if (m_bVoxelized)
		{
			this->GenerateTexture3DMipmap(m_voxelVolume);
			m_bVoxelized = false;
		}
		else if (m_encodedVoxelVolume != nullptr && m_bNeedVoxelize)
		{
			if (bEnableConservativeRasterization)
			{
				glEnable(GL_CONSERVATIVE_RASTERIZATION_NV);
				glConservativeRasterParameterfNV(GL_CONSERVATIVE_RASTER_DILATE_NV, 0.2f);
			}
			else
			{
				glDisable(GL_CONSERVATIVE_RASTERIZATION_NV);
			}

			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

			const unsigned int clearValue = 0;
			m_encodedVoxelVolume->Clear(clearValue);

			m_encodedVoxelizePass->Bind();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);

			// Vertex Shader Uniforms
			m_encodedVoxelizePass->SetMat4f("shadowViewMat", m_shadowViewMat);
			m_encodedVoxelizePass->SetMat4f("shadowProjMat", m_shadowProjMat);

			// Geometry Shader Uniforms
			m_encodedVoxelizePass->SetMat4f("projXAxis", m_projX);
			m_encodedVoxelizePass->SetMat4f("projYAxis", m_projY);
			m_encodedVoxelizePass->SetMat4f("projZAxis", m_projZ);

			// Fragment Shader Uniforms
			m_encodedVoxelizePass->SetInt("shadowMap", 5);
			m_shadowMap->BindAsTexture(5);

			glBindImageTexture(0, m_encodedVoxelVolume->GetID(), 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32UI);
			glViewport(0, 0, VoxelUnitSize, VoxelUnitSize);
			RenderScene(scene, m_encodedVoxelizePass, false, true);

			m_shadowMap->UnbindAsTexture(5);
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glDisable(GL_CONSERVATIVE_RASTERIZATION_NV);

			this->DecodeR32UI(m_encodedVoxelVolume, m_voxelVolume);
			m_bVoxelized = true;
			m_bNeedVoxelize = false;
		}
	}
}

void Renderer::RenderVoxel(const Scene* scene)
{
	if (scene != nullptr)
	{
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
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

void Renderer::VoxelConeTracing(const Scene* scene)
{
	if (scene != nullptr)
	{
		auto lights = scene->GetLights();
		auto lightIntensity = lights[0]->GetIntensity();
		const auto lightDirection = -glm::normalize(lights[0]->LightDirection());
		const float UoL = glm::dot(glm::vec3(0.0f, 1.0f, 0.0f), lightDirection);
		lightIntensity *= UoL;
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(lightIntensity.x, lightIntensity.y, lightIntensity.z, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, m_winWidth, m_winHeight);

		m_vctPass->Bind();

		m_vctPass->SetMat4f("shadowViewMat", m_shadowViewMat);
		m_vctPass->SetMat4f("shadowProjMat", m_shadowProjMat);

		m_shadowMap->BindAsTexture(5);
		m_vctPass->SetInt("shadowMap", 5);

		m_voxelVolume->Bind(6);
		m_vctPass->SetInt("voxelVolume", 6);
		m_vctPass->SetFloat("voxelGridWorldSize", VoxelGridWorldSize);
		m_vctPass->SetFloat("voxelDim", static_cast<float>(VoxelUnitSize));

		/* VCT params */
		m_vctPass->SetFloat("maxDist_VCT", VCTMaxDistance);
		m_vctPass->SetFloat("step_VCT", VCTStep);
		m_vctPass->SetFloat("alphaThreshold_VCT", VCTAlphaThreshold);
		m_vctPass->SetInt("specularSampleNum_VCT", VCTSpecularSampleNum);

		/* Debug flags */
		m_vctPass->SetInt("enableDirectDiffuse", bEnableDirectDiffuse ? 1 : 0);
		m_vctPass->SetInt("enableIndirectDiffuse", bEnableIndirectDiffuse ? 1 : 0);
		m_vctPass->SetInt("enableDirectSpecular", bEnableDirectSpecular ? 1 : 0);
		m_vctPass->SetInt("enableIndirectSpecular", bEnableIndirectSpecular ? 1 : 0);
		m_vctPass->SetInt("debugAmbientOcclusion", bDebugAmbientOcclusion ? 1 : 0);

		RenderScene(scene, m_vctPass);

		m_voxelVolume->Unbind(6);
		m_shadowMap->UnbindAsTexture(5);
	}
}

void Renderer::GenerateTexture3DMipmap(Texture3D* target)
{
	if (target != nullptr)
	{
		const int targetMaxMipLevel = static_cast<int>(target->GetMaxMipLevel());
		if (target->GetMaxMipLevel() > 0)
		{
			m_texture3DReductionRGBA->Bind();

			target->Bind(0);
			m_texture3DReductionRGBA->SetInt("srcTexture", 0);

			for (int base = 0; base < targetMaxMipLevel; base += 2)
			{
				const int genMipLevel0 = base + 1;
				const int genMipLevel1 = base + 2;
				int genMips = 0;
				const int baseMipmapDim = std::pow(2, (targetMaxMipLevel - base));

				if (genMipLevel0 <= targetMaxMipLevel)
				{
					glBindImageTexture(0, target->GetID(), genMipLevel0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);
					++genMips;
				}
				if (genMipLevel1 <= targetMaxMipLevel)
				{
					glBindImageTexture(1, target->GetID(), genMipLevel1, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);
					++genMips;
				}

				m_texture3DReductionRGBA->SetInt("srcMipLevel", base);
				m_texture3DReductionRGBA->SetInt("requireMiplevels", genMips);
				m_texture3DReductionRGBA->SetFloat("texelSize", 1.0f / static_cast<float>(baseMipmapDim));

				unsigned int workGroupNum = static_cast<unsigned int>(baseMipmapDim) / 4;
				if (workGroupNum == 0)
				{
					workGroupNum = 1;
				}

				m_texture3DReductionRGBA->Dispatch(workGroupNum, workGroupNum, workGroupNum);
				glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
			}

			glBindImageTexture(0, 0, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA8);
			glBindImageTexture(1, 0, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA8);
		}
	}
}

void Renderer::DecodeR32UI(Texture3D* src, Texture3D* dest)
{
	if (m_decodeR32UIToRGBA8 != nullptr && src != nullptr && dest != nullptr)
	{
		m_decodeR32UIToRGBA8->Bind();

		glBindImageTexture(0, src->GetID(), 0, GL_TRUE, 0, GL_READ_ONLY, GL_R32UI);
		glBindImageTexture(1, dest->GetID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);

		m_decodeR32UIToRGBA8->Dispatch(src->GetWidth()/8, src->GetHeight()/8, src->GetDepth()/8);
		glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		glBindImageTexture(0, 0, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA8);
		glBindImageTexture(1, 0, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA8);
	}
}

void Renderer::Clear(const glm::vec4& color, bool clearDepth)
{
	GLbitfield clearBit = GL_COLOR_BUFFER_BIT | (clearDepth ? GL_DEPTH_BUFFER_BIT : 0);
	glClearColor(color.r, color.g, color.b, color.a);
	glClear(clearBit);
}
