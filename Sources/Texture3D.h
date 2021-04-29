#pragma once
#include "Rendering.h"

struct Sampler3D
{
   GLint MinFilter = GL_LINEAR_MIPMAP_LINEAR;
   GLint MagFilter = GL_NEAREST;
   GLint WrapS = GL_CLAMP_TO_BORDER;
   GLint WrapT = GL_CLAMP_TO_BORDER;
   GLint WrapR = GL_CLAMP_TO_BORDER;
};

class Texture3D
{
public:
   Texture3D(const std::vector<GLfloat>& rawData, unsigned int width, unsigned int height, unsigned int depth, Sampler3D sampler = Sampler3D(), unsigned int maxMipLevel = 7, bool bGenerateMip = true);
   ~Texture3D() = default;

   void Bind(unsigned int slot);
   void Unbind(unsigned int slot);

   unsigned int GetID() const { return m_id; }

   void Clear(GLfloat clearColor[4]);

private:
   unsigned int m_id = 0;
   
};
