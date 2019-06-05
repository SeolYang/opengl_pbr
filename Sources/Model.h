#pragma once
#include "Object.h"
#include "Rendering.h"

#include <vector>

enum EVertexAttrib
{
	PositionAttrib,
	NormalAttrib,
	TangentAttrib,
	TexcoordsAttrib
};

class Material;
class Texture;
class Sampler;
class Mesh;
class Shader;
class Model : public Object
{
public:
	Model(const std::string& filePath,
		const std::string& name);
	~Model();

	std::string GetFilePath() const { return m_filePath; }
	Material* GetMaterial(size_t idx) const { return m_materials[idx]; }

	void Render(Shader* shader);

private:
	void Init();

	bool LoadGLTFModel(tinygltf::Model& model);

	void LoadTextures(tinygltf::Model& model);
	void LoadMaterials(tinygltf::Model& model);
	void LoadModel(tinygltf::Model& model);

	void ProcessNode(tinygltf::Model& model, tinygltf::Node& node);
	void ProcessMesh(tinygltf::Model& model, tinygltf::Mesh& mesh);

	void RenderNode(
		Shader* shader,
		tinygltf::Model& model,
		tinygltf::Node& node);

private:
	tinygltf::Model m_model;
	std::string m_filePath;
	std::map<unsigned int, GLuint> m_vaos;
	std::map<unsigned int, Texture*> m_textures;
	std::vector<Material*> m_materials;
	std::vector<Mesh*> m_meshes;
	unsigned int m_vao;
	unsigned int m_tanVBO;
};