#pragma once
#include <vector>
#include "Rendering.h"
#include "AABB.h"
#include "Vertex.h"

class Material;
class Shader;
class Mesh
{
public:
	Mesh(std::vector<VertexPosTexNT> vertices, std::vector<unsigned int> indices, Material* material, AABB boundingBox);
	void Render(Shader* shader, GLenum mode = GL_TRIANGLES);

	AABB GetBoundingBox() const
	{
		return m_boundingBox;
	}

private:
	Material*	 m_material;
	unsigned int m_vbo;
	unsigned int m_vao;
	unsigned int m_ebo;
	unsigned int m_count;
	AABB m_boundingBox;

};