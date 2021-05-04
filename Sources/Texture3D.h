#pragma once
#include "Rendering.h"

struct Sampler3D
{
   GLint MinFilter = GL_LINEAR_MIPMAP_LINEAR;
   GLint MagFilter = GL_LINEAR;
   GLint WrapS = GL_CLAMP_TO_BORDER;
   GLint WrapT = GL_CLAMP_TO_BORDER;
   GLint WrapR = GL_CLAMP_TO_BORDER;
};

class Texture3D
{
public:
   Texture3D(const std::vector<GLfloat>& rawData, unsigned int width, unsigned int height, unsigned int depth, Sampler3D sampler = Sampler3D(), unsigned int maxMipLevel = 9, bool bGenerateMip = true);
   Texture3D(const std::vector<GLuint>& rawData, unsigned int width, unsigned int height, unsigned int depth, Sampler3D sampler = Sampler3D(), unsigned int maxMipLevel = 9, bool bGenerateMip = true);
   ~Texture3D() = default;

   void Bind(unsigned int slot);
   void Unbind(unsigned int slot);

   unsigned int GetID() const { return m_id; }

   void Clear(GLfloat clearColor[4]);
   void Clear(unsigned int value = 0);

   unsigned int GetWidth() const { return m_width; }
   unsigned int GetHeight() const { return m_height; }
   unsigned int GetDepth() const { return m_depth; }
   unsigned int GetMaxMipLevel() const { return m_maxMipLevel; }

private:
   unsigned int m_id = 0;
   unsigned int m_width = 0;
   unsigned int m_height = 0;
   unsigned int m_depth = 0;
   unsigned int m_maxMipLevel = 0;
   
};
