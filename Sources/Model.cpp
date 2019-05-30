#include "Model.h"
#include "Texture.h"
#include "Material.h"
#include "Mesh.h"
#include "Rendering.h"
#include "Shader.h"

#include <iostream>

#define BUFFER_OFFSET(idx) ((char *)nullptr + (idx))

Model::Model(const std::string& filePath,
	const std::string& name) :
	m_filePath(filePath),
	Object(name)
{
	Init();
}

Model::~Model()
{
	for (auto mesh : m_meshes)
	{
		if (mesh != nullptr)
		{
			delete mesh;
		}
	}

	for (auto material : m_materials)
	{
		if (material != nullptr)
		{
			delete material;
		}
	}

	for (auto texture : m_textures)
	{
		if (texture != nullptr)
		{
			delete texture;
		}
	}
}

void Model::Render(Shader* shader)
{
	for (auto mesh : m_meshes)
	{
		if (mesh != nullptr)
		{
			shader->SetMat4f("worldMatrix", this->GetWorldMatrix());
			mesh->Render(shader);
		}
	}
}

void Model::Init()
{
	tinygltf::Model model;
	if (LoadGLTFModel(model))
	{
		LoadTextures(model);
		LoadMaterials(model);
		LoadMeshes(model);
	}
}

bool Model::LoadGLTFModel(tinygltf::Model& model)
{
	tinygltf::TinyGLTF loader;

	std::string error;
	std::string warning;

	bool res = loader.LoadASCIIFromFile(&model, &error, &warning, m_filePath);
	if (!warning.empty())
	{
		std::cout << "Warn: " << warning << std::endl;
	}
	if (!error.empty())
	{
		std::cout << "Error: " << error << std::endl;
	}

	if (!res)
	{
		std::cout << "Failed to load gltf Model: " << m_filePath << std::endl;
	}
	else
	{
		std::cout << "Loaded gltf Model: " << m_filePath << std::endl;
	}

	return res;
}

void Model::LoadTextures(tinygltf::Model& model)
{
	for (auto texture : model.textures)
	{
		m_textures.push_back(new Texture{
			model.images[texture.source],
			model.samplers[texture.sampler] });
	}
}

void Model::LoadMaterials(tinygltf::Model& model)
{
	for (auto material : model.materials)
	{
		// PBR Workflow
		// Think about non textured object
		Texture* baseColorTexture = nullptr;
		Texture* metallicRoughnessTexture = nullptr;
		float metallicFactor = 0.0f;
		float roughnessFactor = 0.0f;
		for (auto value : material.values)
		{
			if (value.first.compare("baseColorTexture"))
			{
				baseColorTexture =
					m_textures[value.second.TextureIndex()];
			}
			if (value.first.compare("metallicRoughnessTexture"))
			{
				metallicRoughnessTexture =
					m_textures[value.second.TextureIndex()];
			}
			if (value.first.compare("metallicFactor"))
			{
				metallicFactor = value.second.Factor();
			}
			if (value.first.compare("roughnessFactor"))
			{
				roughnessFactor = value.second.Factor();
			}
		}

		// Addition properties
		for (auto value : material.additionalValues)
		{

		}

		m_materials.push_back(new Material{ baseColorTexture,
			metallicRoughnessTexture,
			metallicFactor,
			roughnessFactor });
	}
}

void Model::LoadMeshes(tinygltf::Model& model)
{
	const int defaultScene = model.defaultScene;
	for (auto node : model.scenes[defaultScene].nodes)
	{
		ProcessNode(node, model);
	}

}

void Model::ProcessNode(int node, tinygltf::Model& model)
{
	const auto targetNode = model.nodes[node];
	ProcessMesh(targetNode.mesh, model);
	for (auto child : targetNode.children)
	{
		ProcessNode(child, model);
	}
}

void Model::ProcessMesh(int mesh, tinygltf::Model& model)
{
	std::map<int, GLuint> vbos;
	auto targetMesh = model.meshes[mesh];
	tinygltf::BufferView bufferView;
	for (size_t idx = 0; idx < model.bufferViews.size(); ++idx)
	{
		bufferView = model.bufferViews[idx];
		if (bufferView.target == 0)
		{
			std::cout << "Warn: bufferView.target is zero" << std::endl;
			continue;
		}

		tinygltf::Buffer buffer = model.buffers[bufferView.buffer];
		std::cout << "BufferView.target " << bufferView.target << std::endl;

		GLuint vbo;
		glGenBuffers(1, &vbo);
		vbos[idx] = vbo; // 만약 bufferView.target == 0 이면 중간에 공백이 생기기에
		glBindBuffer(bufferView.target, vbo);
		glBufferData(bufferView.target, bufferView.byteLength,
			&buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
	}

	for (size_t idx = 0; idx < targetMesh.primitives.size(); ++idx)
	{
		GLuint vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		auto primitive = targetMesh.primitives[idx];
		auto idxAccessor = model.accessors[primitive.indices];

		for (auto& attrib : primitive.attributes)
		{
			tinygltf::Accessor accessor = model.accessors[attrib.second];
			int byteStride = accessor.ByteStride(model.bufferViews[accessor.bufferView]);
			glBindBuffer(GL_ARRAY_BUFFER, vbos[accessor.bufferView]);

			int size = 1;
			if (accessor.type != TINYGLTF_TYPE_SCALAR)
			{
				size = accessor.type;
			}

			int vaa = -1;
			if (attrib.first.compare("POSITION") == 0)
			{
				vaa = 0;
			}
			else if (attrib.first.compare("NORMAL") == 0)
			{
				vaa = 1;
			}
			else if (attrib.first.compare("TEXCOORD_0") == 0)
			{
				vaa = 2;
			}

			if (vaa > -1)
			{
				glEnableVertexAttribArray(vaa);
				glVertexAttribPointer(vaa, size, accessor.componentType,
					accessor.normalized ? GL_TRUE : GL_FALSE, byteStride,
					BUFFER_OFFSET(accessor.byteOffset));
			}
		}

		Mesh* newMesh = new Mesh(m_materials[primitive.material], vao, primitive.mode, idxAccessor.count, idxAccessor.type,
			BUFFER_OFFSET(idxAccessor.byteOffset));
		m_meshes.push_back(newMesh);
	}
}