#include "Mesh.h"
#include "Material.h"
#include "Rendering.h"

Mesh::Mesh(Material* material, unsigned int vao, unsigned int mode, int count, unsigned int type, char* offset) :
	m_material(material),
	m_vao(vao),
	m_mode(mode),
	m_count(count),
	m_type(type),
	m_offset(offset)
{
}

void Mesh::Render(Shader* shader)
{
	if (m_material != nullptr)
	{
		m_material->Bind(shader);
		glBindVertexArray(m_vao);
		glDrawElements(m_mode, m_count, m_type,
			m_offset);
	}
}