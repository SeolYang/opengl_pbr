#include "Model.h"
#include "Texture2D.h"
#include "Material.h"
#include "Mesh.h"
#include "Shader.h"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/pbrmaterial.h>
#include <filesystem>

#include <iostream>

Model::Model(const std::string& name) :
Object(name),
m_mode(GL_TRIANGLES)
{
}

Model::Model(const std::string& name, std::string filePath, const ModelLoadParams& params) :
	Object(name),
   m_mode(GL_TRIANGLES),
	m_filePath(std::move(filePath))
{
	std::filesystem::path path(m_filePath);
	path = path.parent_path();
	std::wstring originParentPath = path.c_str();
	char folderPath[2048];
	size_t converted = 0;
	wcstombs_s(&converted, folderPath, 2048, originParentPath.c_str(), originParentPath.size());
	m_folderPath = folderPath;
	m_folderPath.append("/");
	LoadModel(params);
}

Model::~Model()
{
	for (auto& material : m_materials)
	{
		delete material;
	}
	for (auto& mesh : m_meshes)
	{
		delete mesh;
	}
}

void Model::LoadModel(const ModelLoadParams& params)
{
	Assimp::Importer importer;
	auto scene = importer.ReadFile(m_filePath,
		(params.CalcTangentSpace ? aiProcess_CalcTangentSpace : 0x0) |
		(params.Triangulate ? aiProcess_Triangulate : 0x0) |
		(params.ConvertToLeftHanded ? aiProcess_ConvertToLeftHanded : 0x0) |
		(params.GenUVs ? aiProcess_GenUVCoords : 0x0) |
		(params.PreTransformVertices ? aiProcess_PreTransformVertices : 0x0));

	this->ProcessNode(scene, scene->mRootNode);

	importer.FreeScene();
}

void Model::ProcessNode(const aiScene* scene, const aiNode* node)
{
	if (scene != nullptr && node != nullptr)
	{
		for (size_t idx = 0; idx < node->mNumMeshes; ++idx)
		{
			ProcessMesh(scene, scene->mMeshes[node->mMeshes[idx]]);
		}

		for (size_t childIdx = 0; childIdx < node->mNumChildren; ++childIdx)
		{
			ProcessNode(scene, node->mChildren[childIdx]);
		}
	}
}

void Model::ProcessMesh(const aiScene* scene, const aiMesh* mesh)
{
   if (scene != nullptr && mesh != nullptr)
   {
		AABB meshBoundingBox;

		std::vector<VertexPosTexNT> vertices(mesh->mNumVertices);
		std::vector<unsigned int> indices(mesh->mNumFaces * 3);

		bool bHasUVCoords = mesh->HasTextureCoords(0);
		bool bHasNormals = mesh->HasNormals();
		bool bHasTangentsAndBitangents = mesh->HasTangentsAndBitangents();

		/* Vertices Data Process */
		for (unsigned int idx = 0; idx < mesh->mNumVertices; ++idx)
		{
			auto& vertex = vertices[idx];
			const auto& aiPos = mesh->mVertices[idx];
			vertex.Position = glm::vec3(aiPos.x, aiPos.y, aiPos.z);

			// Model Bounding Box
			this->m_boundingBox.UpdateMin(vertex.Position);
			this->m_boundingBox.UpdateMax(vertex.Position);

			// Mesh Bounding Box
			meshBoundingBox.UpdateMin(vertex.Position);
			meshBoundingBox.UpdateMax(vertex.Position);

			if (bHasUVCoords)
			{
				const auto& aiTexCoords = mesh->mTextureCoords[0][idx];
				vertex.TexCoord = glm::vec2(aiTexCoords.x, aiTexCoords.y);
			}

			if (bHasNormals)
			{
				const auto& aiNormal = mesh->mNormals[idx];
				vertex.Normal = glm::vec3(aiNormal.x, aiNormal.y, aiNormal.z);
			}

			if (bHasTangentsAndBitangents)
			{
				const auto& aiTangent = mesh->mTangents[idx];
				vertex.Tangent = glm::vec3(aiTangent.x, aiTangent.y, aiTangent.z);
			}
		}

		/* Indices Data Process */
		for (size_t idx = 0; idx < mesh->mNumFaces; ++idx)
		{
			const auto& face = mesh->mFaces[idx];
			indices[idx * 3 + 0] = face.mIndices[0];
			indices[idx * 3 + 1] = face.mIndices[1];
			indices[idx * 3 + 2] = face.mIndices[2];
		}

		/* Material Data Process */
		const auto newMat = new Material();
		newMat->SetName(mesh->mName.C_Str());

		aiString baseColorFileName;
		aiString metallicRoughnessFileName;
		aiString aoFileName;
		aiString emissiveFileName;
		aiString normalFileName;

		const auto aiMaterial = scene->mMaterials[mesh->mMaterialIndex];
		aiMaterial->GetTexture(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_BASE_COLOR_TEXTURE, &baseColorFileName);
		aiMaterial->GetTexture(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLICROUGHNESS_TEXTURE, &metallicRoughnessFileName);
		aiMaterial->GetTexture(aiTextureType::aiTextureType_EMISSIVE, 0, &emissiveFileName);
		aiMaterial->GetTexture(aiTextureType::aiTextureType_AMBIENT_OCCLUSION, 0, &aoFileName);
		aiMaterial->GetTexture(aiTextureType::aiTextureType_NORMALS, 0, &normalFileName);

		if (baseColorFileName.length > 0)
		{
			auto baseColorPath = m_folderPath;
			baseColorPath.append(baseColorFileName.C_Str());
			if (const auto baseColor = new Texture2D(baseColorPath); baseColor->GetID() != 0)
			{
				newMat->SetBaseColor(baseColor);
			}
			else
			{
				delete baseColor;
			}
		}

		if (metallicRoughnessFileName.length > 0)
		{
			auto metallicRoughnnesPath = m_folderPath;
			metallicRoughnnesPath.append(metallicRoughnessFileName.C_Str());
			if (const auto metallicRoughness = new Texture2D(metallicRoughnnesPath); metallicRoughness->GetID() != 0)
			{
				newMat->SetMetallicRoughness(metallicRoughness);
			}
			else
			{
				delete metallicRoughness;
			}
		}

		if (emissiveFileName.length > 0)
		{
			auto emissivePath = m_folderPath;
			emissivePath.append(emissiveFileName.C_Str());
			if (const auto emissive = new Texture2D(emissivePath); emissive->GetID() != 0)
			{
				newMat->SetEmissive(emissive);
			}
			else
			{
				delete emissive;
			}
		}

		if (aoFileName.length > 0)
		{
			auto aoPath = m_folderPath;
			aoPath.append(aoFileName.C_Str());
			if (const auto ao = new Texture2D(aoPath); ao->GetID() != 0)
			{
				newMat->SetAmbientOcclusion(ao);
			}
			else
			{
				delete ao;
			}
		}

		if (normalFileName.length > 0 )
		{
			auto normalPath = m_folderPath;
			normalPath.append(normalFileName.C_Str());
			if (const auto normal = new Texture2D(normalPath); normal->GetID() != 0)
			{
				newMat->SetNormal(normal);
			}
			else
			{
				delete normal;
			}
		}

		const auto newMesh = new Mesh(vertices, indices, newMat, meshBoundingBox);
		m_meshes.push_back(newMesh);
		m_materials.push_back(newMat);
   }
}

void Model::Render(Shader* shader)
{
	if (this->IsActivated())
	{
		for (auto& mesh : m_meshes)
		{
			mesh->Render(shader, m_mode);
		}
	}
}