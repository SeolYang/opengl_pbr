#pragma once
#include "Object.h"

#include <vector>

namespace tinygltf
{
	class Model;
}

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
	void LoadMeshes(tinygltf::Model& model);

	void ProcessNode(int node, tinygltf::Model& model);
	void ProcessMesh(int mesh, tinygltf::Model& model);
	
private:
	std::string m_filePath;
	std::vector<Texture*> m_textures;
	std::vector<Material*> m_materials;
	std::vector<Mesh*> m_meshes;

};