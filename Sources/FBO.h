#pragma once
#include "Rendering.h"

class FBO
{
public:
   FBO(unsigned int width, unsigned int height, GLenum magFilter = GL_NEAREST, GLenum minFilter = GL_NEAREST, GLint internalFormat = GL_RGB16F, GLint format = GL_FLOAT, GLint wrap = GL_REPEAT) :
   m_width(width),
   m_height(height)
   {
      GLint prevFB = 0;
      glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFB);

      glGenFramebuffers(1, &m_fbo);
      glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

      glGenTextures(1, &m_colorBuffer);
      glBindTexture(GL_TEXTURE_2D, m_colorBuffer);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

      glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, GL_RGBA, format, nullptr);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorBuffer, 0);

      glGenRenderbuffers(1, &m_rbo);
      glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rbo);

      glBindRenderbuffer(GL_RENDERBUFFER, 0);
      glBindFramebuffer(GL_FRAMEBUFFER, prevFB);
   }

   ~FBO()
   {
      glDeleteTextures(1, &m_colorBuffer);
      glDeleteFramebuffers(1, &m_fbo);
   }

   void Clear()
   {
      GLint prevFB = 0;
      glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFB);

      glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glBindFramebuffer(GL_FRAMEBUFFER, prevFB);
   }

   void Bind()
   {
      glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
      glDrawBuffer(GL_COLOR_ATTACHMENT0);
   }

   void Unbind()
   {
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
   }

   void BindAsTexture(unsigned int slot)
   {
      glActiveTexture(GL_TEXTURE0 + slot);
      glBindTexture(GL_TEXTURE_2D, m_colorBuffer);
   }

   void UnbindAsTexture(unsigned int slot)
   {
      glActiveTexture(GL_TEXTURE0 + slot);
      glBindTexture(GL_TEXTURE_2D, 0);
   }

   unsigned int GetID() const { return m_fbo; }

   unsigned int GetWidth() const { return m_width; }
   unsigned int GetHeight() const { return m_height; }

private:
   unsigned int m_width = 0;
   unsigned int m_height = 0;
   unsigned int m_fbo = 0;
   unsigned int m_colorBuffer = 0;
   unsigned int m_rbo = 0;

};
