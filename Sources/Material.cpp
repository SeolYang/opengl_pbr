#include "Material.h"
#include "Texture.h"
#include "Shader.h"

#include "GL/gl3w.h"

Material::Material(Texture* baseColor,
	Texture* metallicRoughness,
	float metallicFactor,
	float roughnessFactor) :
	m_baseColor(baseColor),
	m_metallicRoughness(metallicRoughness),
	m_metallicFactor(metallicFactor),
	m_roughnessFactor(roughnessFactor)
{
}

void Material::Bind(Shader* shader)
{
	if (shader != nullptr)
	{
		m_baseColor->Bind(BASECOLOR_TEX_SLOT);
		m_metallicRoughness->Bind(METALLIC_ROUGHNESS_TEX_SLOT);

		shader->SetInt("baseColor", BASECOLOR_TEX_SLOT);
		shader->SetInt("metallicRoughness", METALLIC_ROUGHNESS_TEX_SLOT);
		shader->SetFloat("metallicFactor", m_metallicFactor);
		shader->SetFloat("roughnessFactor", m_roughnessFactor);
	}
}