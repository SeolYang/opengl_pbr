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
		glm::vec4 baseColorFactor{ 0.0f };

		Texture* metallicRoughnessTexture = nullptr;
		float metallicFactor = 0.0f;
		float roughnessFactor = 0.0f;

		Texture* ao = nullptr;

		Texture* emissive = nullptr;
		glm::vec3 emissiveFactor{ 0.0f };

		Texture* normal = nullptr;

		int textureIdx = 0;
		for (auto value : material.values)
		{
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
			if (value.first.compare("normalTexture") == 0)
			{
				textureIdx = model.textures[value.second.TextureIndex()].source;
				normal = m_textures[textureIdx];
			}
			else if (value.first.compare("occlusionTexture") == 0)
			{
				textureIdx = model.textures[value.second.TextureIndex()].source;
				ao = m_textures[textureIdx];
			}
			
			else if (value.first.compare("emissiveTexture") == 0)
			{
				textureIdx = model.textures[value.second.TextureIndex()].source;
				emissive = m_textures[textureIdx];
			}
			else if (value.first.compare("emissiveFactor") == 0)
			{
				auto colorFactor = value.second.ColorFactor();
				emissiveFactor.r = colorFactor[0];
				emissiveFactor.g = colorFactor[1];
				emissiveFactor.b = colorFactor[2];
			}
		}

		m_materials.push_back(new Material{ 
			baseColorTexture,
			baseColorFactor,
			normal,
			metallicRoughnessTexture,
			metallicFactor,
			roughnessFactor,
			ao,
			emissive,
			emissiveFactor});
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
	tinygltf::Buffer verticesGLTF;
	tinygltf::BufferView verticesView;
	tinygltf::Buffer indicesGLTF;
	tinygltf::BufferView indicesView;
	tinygltf::Buffer texcoordsGLTF;
	tinygltf::BufferView texcoordsView;

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
			indicesGLTF = buffer;
			indicesView = bufferView;
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
		indicesView.byteStride = idxAccessor.ByteStride(indicesView);

		for (auto& attrib : primitive.attributes)
		{
			tinygltf::Accessor accessor = model.accessors[attrib.second];
			tinygltf::BufferView bufferView = model.bufferViews[accessor.bufferView];
			tinygltf::Buffer buffer = model.buffers[bufferView.buffer];
			int byteStride = accessor.ByteStride(bufferView);
			glBindBuffer(GL_ARRAY_BUFFER, vbos[accessor.bufferView]);

			int size = 1;
			if (accessor.type != TINYGLTF_TYPE_SCALAR)
			{
				size = accessor.type;
			}

			int vaa = -1;
			if (attrib.first.compare("POSITION") == 0)
			{
				verticesGLTF = buffer;
				verticesView = bufferView;
				verticesView.byteStride = byteStride;
				vaa = EVertexAttrib::PositionAttrib;
			}
			else if (attrib.first.compare("NORMAL") == 0)
			{
				vaa = EVertexAttrib::NormalAttrib;
			}
			else if (attrib.first.compare("TEXCOORD_0") == 0)
			{
				texcoordsGLTF = buffer;
				texcoordsView = bufferView;
				texcoordsView.byteStride = byteStride;
				vaa = EVertexAttrib::TexcoordsAttrib;
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

		size_t verticesNum = verticesView.byteLength / verticesView.byteStride;
		size_t texcoordsNum = texcoordsView.byteLength / texcoordsView.byteStride;
		size_t indicesNum = (indicesView.byteLength / indicesView.byteStride);
		size_t trianglesNum = indicesNum / 3;
		std::cout << "#Vertices: " << verticesNum << "\t#Indices: " << indicesNum << "\t#Triangles: " << trianglesNum << std::endl;

		// Tagent space generation
		glm::vec3* vertices = 
			reinterpret_cast<glm::vec3*>(&verticesGLTF.data[0]
				+ verticesView.byteOffset);

		glm::vec2* texcoords =
			reinterpret_cast<glm::vec2*>(&indicesGLTF.data[0]
				+ indicesView.byteOffset);

		std::vector<glm::vec3> tangents;
		tangents.reserve(verticesNum);
		tangents.resize(verticesNum);

		unsigned char* indicesRaw = &indicesGLTF.data[0] + indicesView.byteOffset;
		// Indices must be unsigned (byte | short | int)
		for (size_t idx = 0; idx < indicesNum; idx += 3)
		{
			unsigned int face[3];
			unsigned int vertexIdx = 0;
			switch (indicesView.byteStride)
			{
			case 1:
				face[0] = (unsigned int)(indicesRaw)[idx];
				face[1] = (unsigned int)(indicesRaw)[idx+1];
				face[2] = (unsigned int)(indicesRaw)[idx+2];
				break;
			case 2:
				face[0] = (unsigned int)((unsigned short*)indicesRaw)[idx];
				face[1] = (unsigned int)((unsigned short*)indicesRaw)[idx+1];
				face[2] = (unsigned int)((unsigned short*)indicesRaw)[idx+2];
				break;
			case 4:
				face[0] = ((unsigned int*)indicesRaw)[idx];
				face[1] = ((unsigned int*)indicesRaw)[idx + 1];
				face[2] = ((unsigned int*)indicesRaw)[idx + 2];
				break;
			}

			glm::vec3 edge[2]{
				(vertices[face[1]] - vertices[face[0]]),
				(vertices[face[2]] - vertices[face[0]])
			};
			glm::vec2 deltaUV[2]{
				(texcoords[face[1]] - texcoords[face[0]]),
				(texcoords[face[2]] - texcoords[face[0]])
			};
			float dirCorrection = (deltaUV[1].x * deltaUV[0].y - deltaUV[1].y * deltaUV[0].x) < 0.0f ? -1.0f : 1.0f;

			if (deltaUV[0].x * deltaUV[1].y == deltaUV[0].y * deltaUV[1].x)
			{
				deltaUV[0] = { 0.0f, 1.0f };
				deltaUV[1] = { 1.0f, 0.0f };
			}

			glm::vec3 tangent{
				dirCorrection * (deltaUV[1].y * edge[0].x - deltaUV[0].y * edge[1].x),
				dirCorrection * (deltaUV[1].y * edge[0].y - deltaUV[0].y * edge[1].y),
				dirCorrection * (deltaUV[1].y * edge[0].z - deltaUV[0].y * edge[1].z)
			};

			tangent = glm::normalize(tangent);
			tangents[face[0]] = tangent;
			tangents[face[1]] = tangent;
			tangents[face[2]] = tangent;
		}

		std::cout << "#Tangents: " << tangents.size() << std::endl;
		glGenBuffers(1, &m_tanVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_tanVBO);

		glBufferData(GL_ARRAY_BUFFER, 3*sizeof(float)*tangents.size(),
			&tangents[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(EVertexAttrib::TangentAttrib);
		glVertexAttribPointer(EVertexAttrib::TangentAttrib,
			3, GL_FLOAT,
			GL_FALSE,
			3 * sizeof(float), 0);
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
		glBindBuffer(GL_ARRAY_BUFFER, m_tanVBO);
		glDrawElements(primitive.mode, idxAccessor.count,
			idxAccessor.componentType,
			BUFFER_OFFSET(idxAccessor.byteOffset));
		m_materials[primitive.material]->Unbind(shader);
	}
}