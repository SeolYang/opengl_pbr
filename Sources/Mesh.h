#pragma once
#include "glm/matrix.hpp"

class Material;
class Shader;
class Mesh
{
public:
	Mesh(Material* material,  unsigned int vao,
		unsigned int mode, int count, unsigned int type,
		char* offset);

	void Render(Shader* shader);

private:
	Material*	 m_material;
	unsigned int m_vao;
	unsigned int m_mode;
	int			 m_count;
	unsigned int m_type;
	char*		 m_offset;

};