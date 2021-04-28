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

class Texture2D;
class Shader;
class Material
{
public:
	Material() :
	m_baseColor(nullptr),
	m_metallicRoughness(nullptr),
	m_emissive(nullptr),
	m_ao(nullptr),
	m_normal(nullptr),
	m_baseColorFactor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)),
	m_metallicFactor(0.0f),
	m_roughnessFactor(1.0f),
	m_emissiveFactor(glm::vec3(0.0f, 0.0f, 0.0f))
	{
	}

	Material(
		Texture2D* baseColor,
		glm::vec4 baseColorFactor,
		Texture2D* normal,
		Texture2D* metallicRoughness,
		float metallicFactor,
		float roughnessFactor,
		Texture2D* ao,
		Texture2D* emissive,
		glm::vec3 emissiveFactor);

	~Material();

	void Bind(Shader* shader);
	void Unbind(Shader* shader);

	void SetBaseColor(Texture2D* baseColor) { m_baseColor = baseColor; }
	Texture2D* GetBaseColor() const { return m_baseColor; }

	void SetNormal(Texture2D* normal) { m_normal = normal; }
	Texture2D* GetNormal() const { return m_normal; }

	void SetMetallicRoughness(Texture2D* metallicRoughness) { m_metallicRoughness = metallicRoughness; }
	Texture2D* GetMetallicRoughness() const { return m_metallicRoughness; }

	void SetMetallicFactor(float factor) { m_metallicFactor = factor; }
	float GetMetallicFactor() const { return m_metallicFactor; }

	void SetRoughnessFactor(float factor) { m_roughnessFactor = factor; }
	float GetRoughnessFactor() const { return m_roughnessFactor; }

	void SetAmbientOcclusion(Texture2D* ao) { m_ao = ao; }
	Texture2D* GetAmbientOcclusion() const { return m_ao; }

	void SetEmissive(Texture2D* emissive) { m_emissive = emissive; }
	Texture2D* GetEmissive() const { return m_emissive; }

	void SetEmissiveFactor(const glm::vec3& factor) { m_emissiveFactor = factor; }
	glm::vec3 GetEmissiveFactor() const { return m_emissiveFactor; }

private:
	Texture2D* m_baseColor;
	glm::vec4 m_baseColorFactor;

	Texture2D* m_metallicRoughness;
	float m_metallicFactor;
	float m_roughnessFactor;

	Texture2D* m_ao;

	Texture2D* m_emissive;
	glm::vec3 m_emissiveFactor;

	Texture2D* m_normal;

};