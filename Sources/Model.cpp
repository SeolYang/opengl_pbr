#include "Model.h"
#include "Texture.h"
#include "Material.h"
#include "Mesh.h"
#include "Shader.h"

#include <iostream>

#define BUFFER_OFFSET(idx) ((char *)NULL + (idx))

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
	glBindVertexArray(m_vao);
	const tinygltf::Scene& scene = m_model.scenes[m_model.defaultScene];
	for (size_t idx = 0; idx < m_model.nodes.size(); ++idx)
	{
		RenderNode(shader, m_model, m_model.nodes[idx]);
	}
	glBindVertexArray(0);
}

void Model::Init()
{
	if (LoadGLTFModel(m_model))
	{
		LoadTextures(m_model);
		LoadMaterials(m_model);
		LoadModel(m_model);
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

void Model::LoadModel(tinygltf::Model& model)
{
	std::map<int, GLuint> vbos;
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	const tinygltf::Scene& scene = model.scenes[model.defaultScene];
	for (size_t idx = 0; idx < scene.nodes.size(); ++idx)
	{
		ProcessNode(vbos, model, model.nodes[idx]);
	}

	glBindVertexArray(0);
	for (size_t idx = 0; idx < vbos.size(); ++idx)
	{
		glDeleteBuffers(1, &vbos[idx]);
	}
}

void Model::ProcessNode(std::map<int, GLuint> vbos, tinygltf::Model& model, tinygltf::Node& node)
{
	ProcessMesh(vbos, model, model.meshes[node.mesh]);
	for (size_t idx = 0; idx < node.children.size(); ++idx)
	{
		ProcessNode(vbos, model, model.nodes[node.children[idx]]);
	}
}

std::map<int, GLuint> Model::ProcessMesh(std::map<int, GLuint> vbos, tinygltf::Model& model, tinygltf::Mesh& mesh)
{
	for (size_t idx = 0; idx < model.bufferViews.size(); ++idx)
	{
		const tinygltf::BufferView& bufferView = model.bufferViews[idx];
		if (bufferView.target == 0)
		{
			std::cout << "WARN: BufferView.Target is zero" << std::endl;
			continue;
		}

		tinygltf::Buffer buffer = model.buffers[bufferView.buffer];
		std::cout << "BufferView.target: ";
		switch (bufferView.target)
		{
		case GL_ARRAY_BUFFER:
			std::cout << "GL_ARRAY_BUFFER:";
			break;
		case GL_ELEMENT_ARRAY_BUFFER:
			std::cout << "GL_ELEMENT_ARRAY_BUFFER:";
		}
		std::cout << bufferView.target << std::endl;

		GLuint vbo;
		glGenBuffers(1, &vbo);
		vbos[idx] = vbo;
		glBindBuffer(bufferView.target, vbo);

		std::cout << "Buffer.data.size: " << buffer.data.size()
			<< ", BufferView.byteoffset: " << bufferView.byteOffset << std::endl;

		glBufferData(bufferView.target, bufferView.byteLength,
			&buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
	}

	for (size_t idx = 0; idx < mesh.primitives.size(); ++idx)
	{
		tinygltf::Primitive primitive = mesh.primitives[idx];
		tinygltf::Accessor idxAccessor = model.accessors[primitive.indices];

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
				glVertexAttribPointer(vaa, size, accessor.componentType,
					accessor.normalized ? GL_TRUE : GL_FALSE,
					byteStride, BUFFER_OFFSET(accessor.byteOffset));
				glEnableVertexAttribArray(vaa);
			}
			else
			{
				std::cout << "Unknwon vaa: " << attrib.first << std::endl;
			}
		}
	}

	return vbos;
}

void Model::RenderNode(Shader* shader, tinygltf::Model& model, tinygltf::Node& node)
{
	RenderMesh(shader, model, model.meshes[node.mesh]);
	for (size_t idx = 0; idx < node.children.size(); ++idx)
	{
		RenderNode(shader, model, model.nodes[node.children[idx]]);
	}
}

void Model::RenderMesh(Shader* shader, tinygltf::Model& model, tinygltf::Mesh& mesh)
{
	for (size_t idx = 0; idx < mesh.primitives.size(); ++idx)
	{
		tinygltf::Primitive primitive = mesh.primitives[idx];
		tinygltf::Accessor idxAccessor = model.accessors[primitive.indices];

		m_materials[primitive.material]->Bind(shader);
		glDrawElements(primitive.mode, idxAccessor.count,
			idxAccessor.componentType,
			BUFFER_OFFSET(idxAccessor.byteOffset));
	}
}