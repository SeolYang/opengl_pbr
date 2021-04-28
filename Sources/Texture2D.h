#pragma once
#include "Rendering.h"
#include <string>

namespace tinygltf
{
	struct Image;
	struct Sampler;
};

struct Sampler2D
{
	GLint MinFilter = GL_LINEAR_MIPMAP_LINEAR;
	GLint MagFilter = GL_LINEAR;
	GLint WrapS = GL_REPEAT;
	GLint WrapT = GL_REPEAT;
};

class Texture2D
{
public:
	Texture2D(std::string_view filePath, Sampler2D sampler = Sampler2D(), bool bGenerateMip = true);
	Texture2D(tinygltf::Image& image, tinygltf::Sampler& sampler);

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