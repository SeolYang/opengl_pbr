#pragma once
#include "Object.h"
#include "Rendering.h"
#include "AABB.h"

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
struct aiScene;
struct aiMesh;
struct aiNode;

struct ModelLoadParams
{
public:
	bool CalcTangentSpace = true;
	bool ConvertToLeftHanded = false;
	bool GenSmoothNormals = true;
	bool GenUVs = true;
	bool PreTransformVertices = true;
	bool Triangulate = true;
};

class Model : public Object
{
public:
	Model(const std::string& name);
	Model(const std::string& name, std::string filePath, const ModelLoadParams& params);
	~Model();

	std::string GetFilePath() const { return m_filePath; }
	Material* GetMaterial(size_t idx) const { return m_materials[idx]; }
	const std::vector<Material*>& GetMaterials() const { return m_materials; }

	std::vector<Material*>& GetMaterials() { return m_materials; }
	std::vector<Mesh*>& GetMeshes() { return m_meshes; }

	void Render(Shader* shader);

	void SetMode(GLenum mode = GL_TRIANGLES) { m_mode = mode; }
	GLenum GetMode() const { return m_mode; }

	AABB GetBoundingBox(bool worldSpace = true) const
	{
		if (worldSpace)
		{
			const auto worldMatrix = GetWorldMatrix();
			return AABB(
				glm::vec3(worldMatrix * glm::vec4(m_boundingBox.Min, 1.0f)),
				glm::vec3(worldMatrix * glm::vec4(m_boundingBox.Max, 1.0f)));
		}

		return m_boundingBox;
	}

private:
	void LoadModel(const ModelLoadParams& params);
	void ProcessNode(const aiScene* scene, const aiNode* node);
	void ProcessMesh(const aiScene* scene, const aiMesh* mesh);

public:
	bool bCastShadow = true;
	bool bDoubleSided = false;

protected:
	AABB m_boundingBox;

private:
	std::string m_filePath;
	std::string m_folderPath;
	std::vector<Material*> m_materials;
	std::vector<Mesh*> m_meshes;
	GLenum m_mode;

};