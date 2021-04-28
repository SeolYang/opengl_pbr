#include "Material.h"
#include "Texture2D.h"
#include "Shader.h"

#include "GL/gl3w.h"

Material::Material(
	Texture2D* baseColor,
	glm::vec4 baseColorFactor,
	Texture2D* normal,
	Texture2D* metallicRoughness,
	float metallicFactor,
	float roughnessFactor,
	Texture2D* ao,
	Texture2D* emissive,
	glm::vec3 emissiveFactor) :
	m_baseColor(baseColor),
	m_baseColorFactor(baseColorFactor),
	m_normal(normal),
	m_metallicRoughness(metallicRoughness),
	m_metallicFactor(metallicFactor),
	m_roughnessFactor(roughnessFactor),
	m_ao(ao),
	m_emissive(emissive),
	m_emissiveFactor(emissiveFactor)
{
}

Material::~Material()
{
	delete m_baseColor;
	delete m_normal;
	delete m_ao;
	delete m_metallicRoughness;
	delete m_emissive;
}

void Material::Bind(Shader* shader)
{
	if (shader != nullptr)
	{
		if (m_baseColor != nullptr)
		{
			m_baseColor->Bind(EMaterialTextureSlot::BaseColor);
			shader->SetVec4f("baseColorFactor", glm::vec4(0.0f));
		}
		else
		{
			shader->SetVec4f("baseColorFactor", m_baseColorFactor);
		}
		shader->SetInt("baseColorMap", EMaterialTextureSlot::BaseColor);

		if (m_normal != nullptr)
		{
			m_normal->Bind(EMaterialTextureSlot::Normal);
			shader->SetInt("bUseNormalMap", 1);
		}
		else
		{
			shader->SetInt("bUseNormalMap", 0);
		}
		shader->SetInt("normalMap", EMaterialTextureSlot::Normal);

		if (m_metallicRoughness != nullptr)
		{
			m_metallicRoughness->Bind(EMaterialTextureSlot::MetallicRoughness);
			shader->SetFloat("metallicFactor", 0.0f);
			shader->SetFloat("roughnessFactor", 0.0f);
		}
		else
		{
			shader->SetFloat("metallicFactor", m_metallicFactor);
			shader->SetFloat("roughnessFactor", m_roughnessFactor);
		}
		shader->SetInt("metallicRoughnessMap", EMaterialTextureSlot::MetallicRoughness);

		if (m_ao != nullptr)
		{
			m_ao->Bind(EMaterialTextureSlot::AO);
		}

		shader->SetInt("aoMap", EMaterialTextureSlot::AO);

		if (m_emissive != nullptr)
		{
			m_emissive->Bind(EMaterialTextureSlot::Emissive);
			shader->SetVec3f("emissiveFactor", glm::vec3(0.0f));
		}
		else
		{
			shader->SetVec3f("emissiveFactor", m_emissiveFactor);
		}
		shader->SetInt("emissiveMap", EMaterialTextureSlot::Emissive);

	}
}

void Material::Unbind(Shader* shader)
{
	if (shader != nullptr)
	{
		if (m_baseColor != nullptr)
		{
			m_baseColor->Unbind();
		}

		if (m_normal != nullptr)
		{
			m_normal->Unbind();
		}

		if (m_metallicRoughness != nullptr)
		{
			m_metallicRoughness->Unbind();
		}

		if (m_ao != nullptr)
		{
			m_ao->Unbind();
		}

		if (m_emissive != nullptr)
		{
			m_emissive->Unbind();
		}

		shader->SetVec4f("baseColorFactor", glm::vec4(0.0f));
		shader->SetFloat("metallicFactor", 0.0f);
		shader->SetFloat("roughnessFactor", 0.0f);
		shader->SetVec3f("emissiveFactor", glm::vec3(0.0f));
	}
}