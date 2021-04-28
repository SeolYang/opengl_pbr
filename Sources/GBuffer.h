#pragma once

#include "Rendering.h"

class GBuffer
{
public:
	GBuffer(unsigned int width,
		unsigned int height);

	bool Init();

	void BindFrameBuffer();
	void UnbindFrameBuffer();
	void BindTextures();
	void UnbindTextures();

private:
	unsigned int m_width;
	unsigned int m_height;
	unsigned int m_gBuffer;
	unsigned int m_pos;
	unsigned int m_normal;
	unsigned int m_albedo;
	unsigned int m_metallicRoughness;
	unsigned int m_emissiveAO;
	unsigned int m_depth;

};