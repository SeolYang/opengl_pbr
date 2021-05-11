#include "Plane.h"
#include "Mesh.h"
#include "Material.h"

Plane::Plane(std::string_view name) :
Model(std::string(name))
{
   Construct();
}

void Plane::Construct()
{
   auto& meshes = this->GetMeshes();
   auto& materials = this->GetMaterials();

   auto newMat = new Material();
   newMat->SetName(this->GetName() + "_Mat");
   materials.push_back(newMat);

   std::vector<VertexPosTexNT> vertices;
   vertices.resize(4);
   const std::vector<unsigned int> indices  = { 1, 2, 0, 1, 3, 2 };

   const glm::vec3 normal(0.0f, 0.0f, 1.0f);
   const glm::vec3 tangent(1.0f, 0.0f, 0.0f);

   vertices[0] = VertexPosTexNT(
         glm::vec3(-1.0f, 1.0f, 0.0f),
         glm::vec2(0.0f, 0.0f),
         normal,
         tangent);

   vertices[1] = VertexPosTexNT(
      glm::vec3(-1.0f, -1.0f, 0.0f),
      glm::vec2(0.0f, 1.0f),
      normal,
      tangent);

   vertices[2] = VertexPosTexNT(
      glm::vec3(1.0f, 1.0f, 0.0f),
      glm::vec2(1.0f, 0.0f),
      normal,
      tangent);

   vertices[3] = VertexPosTexNT(
      glm::vec3(1.0f, -1.0f, 0.0f),
      glm::vec2(1.0f, 1.0f),
      normal,
      tangent);

   AABB aabb = AABB(glm::vec3(-1.0f, -1.0f, -0.001f), glm::vec3(1.0f, 1.0f, 0.001f));
   auto quad = new Mesh(vertices, indices, newMat, aabb);
   meshes.push_back(quad);

   this->m_boundingBox = aabb;
}
