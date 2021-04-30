#pragma once
#include "Rendering.h"
#include "glm/glm.hpp"

constexpr unsigned int MaximumLights = 128;
// Voxel Volume Texture Size
constexpr unsigned int VoxelUnitSize = 128;
constexpr float VoxelGridWorldSize = 128.0;
constexpr unsigned int VoxelNum = VoxelUnitSize * VoxelUnitSize * VoxelUnitSize;
constexpr float VoxelSize = (VoxelGridWorldSize / static_cast<float>(VoxelUnitSize));
constexpr unsigned int ShadowMapRes = 4096;

enum class ERenderMode
{
   VCT,
	Deferred,
	VoxelVisualization
};

class Scene;
class Mesh;
class Model;
class Light;
class Camera;
class Shader;
class Viewport;
class GBuffer;
class Texture2D;
class Texture3D;
class FBO;
class ShadowMap;
class Renderer
{
public:
	Renderer() = default;
	~Renderer();

	bool Init(unsigned int width, unsigned int height);
	void Render(const Scene* scene);

	void Clear(const glm::vec4& color, bool clearDepth = true);

	void SetRenderMode(ERenderMode mode) { m_renderMode = mode; }
	ERenderMode GetRenderMode() const { return m_renderMode; }

	void SetVoxelizeEveryFrame(bool bEnable) { m_bAlwaysComputeVoxel = bEnable; }
	bool IsVoxelizeEveryFrame() const { return m_bAlwaysComputeVoxel; }

private:
	void DeferredRender(const Scene* scene);

	void Shadow(const Scene* scene);

	void Voxelize(const Scene* scene);
	void RenderVoxel(const Scene* scene);
	void VoxelConeTracing(const Scene* scene);

private:
	ERenderMode m_renderMode = ERenderMode::Deferred;
	// Deferred Rendering
	GBuffer*	m_gBuffer = nullptr;
	Shader*	m_geometryPass = nullptr;
	Shader*	m_lightingPass = nullptr;

	// Shadow Mapping
	ShadowMap* m_shadowMap = nullptr;
	Shader* m_shadowPass = nullptr;
	glm::mat4 m_shadowViewMat = glm::mat4();
	glm::mat4 m_shadowProjMat = glm::mat4();

	// Voxel Cone Tracing
	Texture3D*	m_voxelVolume = nullptr;
	Shader*		m_voxelizePass = nullptr;
	glm::mat4 m_projX;
	glm::mat4 m_projY;
	glm::mat4 m_projZ;

	Shader* m_renderVoxelPass = nullptr;
	GLuint m_texture3DVAO = 0;

	Shader* m_vctPass = nullptr;

	unsigned int m_quadVAO = 0;
	unsigned int m_quadVBO = 0;

	unsigned int m_winWidth = 0;
	unsigned int m_winHeight = 1;

	bool m_bAlwaysComputeVoxel = false;
	bool m_bFirstVoxelize = true;
	bool m_bFirstShadow = true;

};