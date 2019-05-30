#include "Texture.h"
#include "Rendering.h"

Texture::Texture(tinygltf::Image& image, tinygltf::Sampler& sampler) :
	m_latestSlot(0)
{
	glGenTextures(1, &m_id);

	glBindTexture(GL_TEXTURE_2D, m_id);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexParameteri(
		GL_TEXTURE_2D,
		GL_TEXTURE_MIN_FILTER,
		sampler.minFilter);
	glTexParameteri(
		GL_TEXTURE_2D, 
		GL_TEXTURE_MAG_FILTER,
		sampler.magFilter);
	glTexParameteri(
		GL_TEXTURE_2D,
		GL_TEXTURE_WRAP_S,
		sampler.wrapS);
	glTexParameteri(
		GL_TEXTURE_2D,
		GL_TEXTURE_WRAP_T,
		sampler.wrapT);

	GLenum format = GL_RGBA;
	if (image.component == 1)
	{
		format = GL_RED;
	}
	else if (image.component == 2)
	{
		format = GL_RG;
	}
	else if (image.component == 3)
	{
		format = GL_RGB;
	}

	GLenum type = GL_UNSIGNED_BYTE;
	if (image.bits == 16)
	{
		type = GL_UNSIGNED_SHORT;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height,
		0, format, type, &image.image.at(0));
}

void Texture::Bind(unsigned int slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_id);
	m_latestSlot = slot;
}