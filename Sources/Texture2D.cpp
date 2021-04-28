#include "Texture2D.h"
#include <iostream>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif

Texture2D::Texture2D(std::string_view filePath, Sampler2D sampler, bool bGenerateMip) :
m_id(0),
m_latestSlot(0),
m_uri(filePath)
{
	int width = 0;
	int height = 0;
	int channels = 0;
	unsigned char* data = stbi_load(filePath.data(), &width, &height, &channels, 0);
	if (data != nullptr)
	{
		glGenTextures(1, &m_id);
		glBindTexture(GL_TEXTURE_2D, m_id);

		glTexParameteri(
			GL_TEXTURE_2D,
			GL_TEXTURE_MIN_FILTER,
			sampler.MinFilter);
		glTexParameteri(
			GL_TEXTURE_2D,
			GL_TEXTURE_MAG_FILTER,
			sampler.MagFilter);
		glTexParameteri(
			GL_TEXTURE_2D,
			GL_TEXTURE_WRAP_S,
			sampler.WrapS);
		glTexParameteri(
			GL_TEXTURE_2D,
			GL_TEXTURE_WRAP_T,
			sampler.WrapT);

		GLenum format = GL_RGB;
		switch(channels)
		{
		case 1:
			format = GL_RED;
			break;

		case 2:
			format = GL_RG;
			break;

		case 4:
			format = GL_RGBA;
			break;
		default:
			break;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		if (bGenerateMip)
		{
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Failed to load texture : " << filePath << std::endl;
	}
}

Texture2D::Texture2D(tinygltf::Image& image, tinygltf::Sampler& sampler) :
	m_uri(image.uri),
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
	glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture2D::Bind(unsigned int slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_id);
	m_latestSlot = slot;
}

void Texture2D::Unbind()
{
	glActiveTexture(GL_TEXTURE0 + m_latestSlot);
	glBindTexture(GL_TEXTURE_2D, 0);
	m_latestSlot = 0;
}