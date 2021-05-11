#include "Mesh.h"
#include "Material.h"
#include "Shader.h"
#include "Rendering.h"

Mesh::Mesh(std::vector<VertexPosTexNT> vertices, std::vector<unsigned int> indices, Material* material, AABB boundingBox) :
m_material(material),
m_vbo(0),
m_vao(0),
m_ebo(0),
m_count(indices.size()),
m_boundingBox(boundingBox)
{
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ebo);
	glGenVertexArrays(1, &m_vao);

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexPosTexNT) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPosTexNT), reinterpret_cast<void*>(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPosTexNT), reinterpret_cast<void*>(sizeof(VertexPosTexNT::Position)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPosTexNT), reinterpret_cast<void*>(sizeof(VertexPosTexNT::Position) + sizeof(VertexPosTexNT::TexCoord)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPosTexNT), reinterpret_cast<void*>(sizeof(VertexPosTexNT::Position) + sizeof(VertexPosTexNT::TexCoord) + sizeof(VertexPosTexNT::Tangent)));
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void Mesh::Render(Shader* shader, GLenum mode)
{
	if (m_material != nullptr)
	{
		m_material->Bind(shader);
		glBindVertexArray(m_vao);

		glDrawElements(mode, m_count, GL_UNSIGNED_INT, nullptr);

		glBindVertexArray(0);
		m_material->Unbind(shader);
	}
}