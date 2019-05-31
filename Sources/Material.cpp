#include "Material.h"
#include "Texture.h"
#include "Shader.h"

#include "GL/gl3w.h"

Material::Material(
	Texture* baseColor,
	glm::vec4 baseColorFactor,
	Texture* metallicRoughness,
	float metallicFactor,
	float roughnessFactor) :
	m_baseColor(baseColor),
	m_baseColorFactor(baseColorFactor),
	m_metallicRoughness(metallicRoughness),
	m_metallicFactor(metallicFactor),
	m_roughnessFactor(roughnessFactor)
{
}

void Material::Bind(Shader* shader)
{
	if (shader != nullptr)
	{
		if (m_baseColor != nullptr)
		{
			m_baseColor->Bind(BASECOLOR_TEX_SLOT);
			shader->SetVec4f("baseColorFactor", glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
		}
		else
		{
			shader->SetVec4f("baseColorFactor", m_baseColorFactor);
		}
		if (m_metallicRoughness != nullptr)
		{
			m_metallicRoughness->Bind(METALLIC_ROUGHNESS_TEX_SLOT);
		}

		shader->SetInt("baseColor", BASECOLOR_TEX_SLOT);
		shader->SetInt("metallicRoughness", METALLIC_ROUGHNESS_TEX_SLOT);
		shader->SetFloat("metallicFactor", m_metallicFactor);
		shader->SetFloat("roughnessFactor", m_roughnessFactor);
	}
}