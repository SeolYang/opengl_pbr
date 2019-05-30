#include "Viewport.h"

#include "GL/gl3w.h"

void Viewport::Bind()
{
	glViewport(m_x, m_y, m_width, m_height);
}