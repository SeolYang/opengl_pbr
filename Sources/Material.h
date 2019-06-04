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

};