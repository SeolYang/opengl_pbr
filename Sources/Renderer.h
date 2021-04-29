#pragma once
#include "Rendering.h"
#include "glm/vec4.hpp"

constexpr unsigned int MaximumLights = 128;
constexpr unsigned int VoxelUnitSize = 256;

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

	void Voxelize(const Scene* scene);
	void VisualizeVoxel(const Scene* scene);

private:
	ERenderMode m_renderMode = ERenderMode::VoxelVisualization;
	// Deferred Rendering
	GBuffer*	m_gBuffer = nullptr;
	Shader*	m_geometryPass = nullptr;
	Shader*	m_lightingPass = nullptr;

	// Voxel Cone Tracing
	Texture3D*	m_voxelVolume = nullptr;
	Shader*		m_voxelizePass = nullptr;

	Shader* m_worldPosPass = nullptr;
	FBO* m_cubeBack = nullptr;
	FBO* m_cubeFront = nullptr;
	Shader* m_visualizeVoxelPass = nullptr;
	Model* m_cube = nullptr;

	unsigned int m_quadVAO = 0;
	unsigned int m_quadVBO = 0;

	unsigned int m_winWidth = 0;
	unsigned int m_winHeight = 1;

	bool m_bAlwaysComputeVoxel = true;

};