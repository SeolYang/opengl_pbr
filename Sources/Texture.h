#pragma once
#include <string>

namespace tinygltf
{
	struct Image;
	struct Sampler;
};

class Texture
{
public:
	Texture(tinygltf::Image& image, tinygltf::Sampler& sampler);

	unsigned int GetID() const { return m_id; }
	unsigned int GetBoundedSlot() const { return m_latestSlot; }

	void Bind(unsigned int slot);
	void Unbind();

	std::string GetURI() const { return m_uri; }

private:
	unsigned int m_id;
	unsigned int m_latestSlot;
	std::string	 m_uri;

};