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
		delete texture.second;
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
	int samplerSize = model.samplers.size();
	for (auto texture : model.textures)
	{
		int source = texture.source;
		int sampler = texture.sampler;

		tinygltf::Sampler targetSampler;
		targetSampler.minFilter = GL_LINEAR;
		targetSampler.magFilter = GL_LINEAR;
		targetSampler.wrapS = GL_REPEAT;
		targetSampler.wrapT = GL_REPEAT;
		targetSampler.wrapR = GL_REPEAT;

		bool bValidSampler = (samplerSize > 0 && samplerSize > texture.sampler);
		if (bValidSampler)
		{
			targetSampler = model.samplers[texture.sampler];
		}

		m_textures[source] = new Texture{
			model.images[source],
			targetSampler };
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
		glm::vec4 baseColorFactor{ 0.0f };
		float metallicFactor = 0.0f;
		float roughnessFactor = 0.0f;
		for (auto value : material.values)
		{
			int textureIdx = 0;
			if (value.first.compare("baseColorTexture") == 0)
			{
				textureIdx = model.textures[value.second.TextureIndex()].source;
				baseColorTexture =
					m_textures[textureIdx];
			}
			else if (value.first.compare("baseColorFactor") == 0)
			{
				auto colorFactor = value.second.ColorFactor();
				baseColorFactor.r = colorFactor[0];
				baseColorFactor.g = colorFactor[1];
				baseColorFactor.b = colorFactor[2];
				if (colorFactor.size() >= 4)
				{
					baseColorFactor.a = colorFactor[3];
				}
			}
			else if (value.first.compare("metallicRoughnessTexture") == 0)
			{
				textureIdx = model.textures[value.second.TextureIndex()].source;
				metallicRoughnessTexture =
					m_textures[textureIdx];
			}
			else if (value.first.compare("metallicFactor") == 0)
			{
				metallicFactor = value.second.Factor();
			}
			else if (value.first.compare("roughnessFactor") == 0)
			{
				roughnessFactor = value.second.Factor();
			}
		}

		// Addition properties
		for (auto value : material.additionalValues)
		{

		}

		m_materials.push_back(new Material{ 
			baseColorTexture,
			baseColorFactor,
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
	if (node.mesh != -1)
	{
		ProcessMesh(vbos, model, model.meshes[node.mesh]);
	}
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
				glEnableVertexAttribArray(vaa);
				glVertexAttribPointer(vaa, size, accessor.componentType,
					accessor.normalized ? GL_TRUE : GL_FALSE,
					byteStride, BUFFER_OFFSET(accessor.byteOffset));
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
	if (node.mesh != -1)
	{
		RenderMesh(shader, model, model.meshes[node.mesh]);
	}
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