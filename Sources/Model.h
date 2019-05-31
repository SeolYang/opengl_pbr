#pragma once
#include "Object.h"
#include "Rendering.h"

#include <vector>

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

	void Render(Shader* shader);

private:
	void Init();

	bool LoadGLTFModel(tinygltf::Model& model);

	void LoadTextures(tinygltf::Model& model);
	void LoadMaterials(tinygltf::Model& model);
	void LoadModel(tinygltf::Model& model);
	void LoadMeshes(tinygltf::Model& model);

	void ProcessNode(std::map<int, GLuint> vbos, tinygltf::Model& model, tinygltf::Node& node);
	std::map<int, GLuint> ProcessMesh(std::map<int, GLuint> vbos, tinygltf::Model& model, tinygltf::Mesh& mesh);

	void RenderNode(Shader* shader, tinygltf::Model& model, tinygltf::Node& node);
	void RenderMesh(Shader* shader, tinygltf::Model& model, tinygltf::Mesh& mesh);

private:
	tinygltf::Model m_model;
	std::string m_filePath;
	std::vector<Texture*> m_textures;
	std::vector<Material*> m_materials;
	std::vector<Mesh*> m_meshes;
	unsigned int m_vao;

};