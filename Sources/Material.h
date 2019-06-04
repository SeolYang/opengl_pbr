#pragma once
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

enum EMaterialTextureSlot
{
	BaseColor,
	Normal,
	MetallicRoughness,
	AO,
	Emissive
};

class Texture;
class Shader;
class Material
{
public:
	Material(
		Texture* baseColor,
		glm::vec4 baseColorFactor,
		Texture* normal,
		Texture* metallicRoughness,
		float metallicFactor,
		float roughnessFactor,
		Texture* ao,
		Texture* emissive,
		glm::vec3 emissiveFactor);

	void Bind(Shader* shader);
	void Unbind(Shader* shader);

	void SetBaseColor(Texture* baseColor) { m_baseColor = baseColor; }
	Texture* GetBaseColor() const { return m_baseColor; }

	void SetNormal(Texture* normal) { m_normal = normal; }
	Texture* GetNormal() const { return m_normal; }

	void SetUseNormal(bool bUseNormal) { m_bUseNormal = bUseNormal; }
	bool IsUseNormal() const { return m_bUseNormal; }

	void SetMetallicRoughness(Texture* metallicRoughness) { m_metallicRoughness = metallicRoughness; }
	Texture* GetMetallicRoughness() const { return m_metallicRoughness; }

	void SetMetallicFactor(float factor) { m_metallicFactor = factor; }
	float GetMetallicFactor() const { return m_metallicFactor; }

	void SetRoughnessFactor(float factor) { m_roughnessFactor = factor; }
	float GetRoughnessFactor() const { return m_roughnessFactor; }

	void SetAmbientOcclusion(Texture* ao) { m_ao = ao; }
	Texture* GetAmbientOcclusion() const { return m_ao; }

	void SetEmissive(Texture* emissive) { m_emissive = emissive; }
	Texture* GetEmissive() const { return m_emissive; }

	void SetEmissiveFactor(const glm::vec3& factor) { m_emissiveFactor = factor; }
	glm::vec3 GetEmissiveFactor() const { return m_emissiveFactor; }

private:
	Texture* m_baseColor;
	glm::vec4 m_baseColorFactor;

	Texture* m_metallicRoughness;
	float m_metallicFactor;
	float m_roughnessFactor;

	Texture* m_ao;

	Texture* m_emissive;
	glm::vec3 m_emissiveFactor;

	Texture* m_normal;
	bool	m_bUseNormal;

};