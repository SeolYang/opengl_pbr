#pragma once
#include "Rendering.h"
#include <iostream>

class ShadowMap
{
public:
   ShadowMap(unsigned int width = 4096, unsigned int height = 4096) :
   m_width(width),
   m_height(height)
   {
      glGenFramebuffers(1, &m_fbo);
      glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

      glGenTextures(1, &m_texture);
      glBindTexture(GL_TEXTURE_2D, m_texture);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

      glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_texture, 0);
      glDrawBuffer(GL_NONE);

      if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
         std::cout << "Error creating framebuffer" << std::endl;
      }
   }

   void Bind()
   {
      glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
   }

   void Unbind()
   {
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
   }

   void BindAsTexture(unsigned int slot)
   {
      glActiveTexture(GL_TEXTURE0 + slot);
      glBindTexture(GL_TEXTURE_2D, m_texture);
   }

   void UnbindAsTexture(unsigned int slot)
   {
      glActiveTexture(GL_TEXTURE0 + slot);
      glBindTexture(GL_TEXTURE_2D, 0);
   }

   unsigned int GetWidth() const { return m_width; }
   unsigned int GetHeight() const { return m_height; }

private:
   unsigned int m_texture = 0;
   unsigned int m_fbo = 0;
   unsigned int m_width = 0;
   unsigned int m_height = 0;

};
