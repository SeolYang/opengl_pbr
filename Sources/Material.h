#pragma once

constexpr unsigned int BASECOLOR_TEX_SLOT = 0;
constexpr unsigned int METALLIC_ROUGHNESS_TEX_SLOT = 1;

class Texture;
class Shader;
class Material
{
public:
	Material(Texture* baseColor,
		Texture* metallicRoughness,
		float metallicFactor,
		float roughnessFactor);

	void Bind(Shader* shader);

private:
	Texture* m_baseColor;
	Texture* m_metallicRoughness;
	float m_metallicFactor;
	float m_roughnessFactor;

};