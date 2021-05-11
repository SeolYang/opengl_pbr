#pragma once
#include "Rendering.h"
#include "glm/glm.hpp"

// Voxel Volume Texture Size
constexpr unsigned int VoxelUnitSize = 512;
constexpr float VoxelGridWorldSize = 512;
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

	void PrintVCTParams() const;

private:
	void RenderScene(const Scene* scene, Shader* shader, bool bIsShadowCasting = false, bool bForceCullFace = false);
	void DeferredRender(const Scene* scene);

	void Shadow(const Scene* scene);

	void Voxelize(const Scene* scene);
	void EncodedVoxelize(const Scene* scene);
	void RenderVoxel(const Scene* scene);
	void VoxelConeTracing(const Scene* scene);

	// 이미 생성 과정에서 mipmap generation이 되었다고 가정
	void GenerateTexture3DMipmap(Texture3D* target);
	void DecodeR32UI(Texture3D* src, Texture3D* dest);

	void DebugConeDirections(const Scene* scene);
	void DebugBoundingBoxes(const Scene* scene);

public:
	bool bEnableDirectDiffuse = true;
	bool bEnableIndirectDiffuse = true;
	bool bEnableDirectSpecular = true;
	bool bEnableIndirectSpecular = true;
	bool bDebugAmbientOcclusion = false;
	bool bDebugConeDirection = false;
	bool bAlwaysVoxelize = false;
	bool bEnableConservativeRasterization = false;
	bool bDebugBoundingBox = false;
	glm::vec3 BoundingBoxDebugColor = glm::vec3(0.0f, 1.0f, 0.0f);

	float VCTMaxDistance = 150.0f;
	float VCTStep = 0.5;
	float VCTAlphaThreshold = 0.98f;
	float VCTInitialStep = 3.5f;
	unsigned int VCTSpecularSampleNum = 2;

	float DebugConeLength = 1.5f;

private:
	ERenderMode m_renderMode = ERenderMode::VCT;
	// Deferred Rendering
	GBuffer*	m_gBuffer = nullptr;
	Shader*	m_geometryPass = nullptr;
	Shader*	m_lightingPass = nullptr;

	// Shadow Mapping
	bool m_bFirstShadow = true;
	ShadowMap* m_shadowMap = nullptr;
	Shader* m_shadowPass = nullptr;
	glm::mat4 m_shadowViewMat = glm::mat4();
	glm::mat4 m_shadowProjMat = glm::mat4();

	// Voxel Cone Tracing
	bool m_bFirstVoxelize = true;
	Texture3D*	m_encodedVoxelVolume = nullptr;
	Texture3D*	m_voxelVolume = nullptr;
	Shader* m_encodedVoxelizePass = nullptr;
	Shader*		m_voxelizePass = nullptr;
	glm::mat4 m_projX;
	glm::mat4 m_projY;
	glm::mat4 m_projZ;

	GLuint m_texture3DVAO = 0;
	GLuint m_boundingBoxPointVAO = 0;

	Shader* m_vctPass = nullptr;

	unsigned int m_quadVAO = 0;
	unsigned int m_quadVBO = 0;

	unsigned int m_winWidth = 0;
	unsigned int m_winHeight = 1;

	bool m_bVoxelized = false;
	bool m_bNeedVoxelize = true;

	Shader* m_texture3DReductionRGBA = nullptr;
	Shader* m_decodeR32UIToRGBA8 = nullptr;

	/* Debug */
	Shader* m_renderVoxelPass = nullptr;
	Shader* m_visualizeConeDirPass = nullptr;
	Shader* m_visualizeBoundingBoxPass = nullptr;

};