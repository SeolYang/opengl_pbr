#pragma once
#include <glm/glm.hpp>
#include <numeric>

struct AABB
{
public:
	void Combine(const AABB& other)
	{
		const glm::vec3 newMin{
			(other.Min.x < Min.x ? other.Min.x : Min.x),
			(other.Min.y < Min.y ? other.Min.y : Min.y),
			(other.Min.z < Min.z ? other.Min.z : Min.z) };
		const glm::vec3 newMax{
			(other.Max.x > Max.x ? other.Max.x : Max.x),
			(other.Max.y > Max.y ? other.Max.y : Max.y),
			(other.Max.z > Max.z ? other.Max.z : Max.z) };

		Min = newMin;
		Max = newMax;
	}

	AABB Combined(const AABB& other) const
	{
		const glm::vec3 newMin{
			(other.Min.x < Min.x ? other.Min.x : Min.x),
			(other.Min.y < Min.y ? other.Min.y : Min.y),
			(other.Min.z < Min.z ? other.Min.z : Min.z) };
		const glm::vec3 newMax{
			(other.Max.x > Max.x ? other.Max.x : Max.x),
			(other.Max.y > Max.y ? other.Max.y : Max.y),
			(other.Max.z > Max.z ? other.Max.z : Max.z) };

		return AABB(newMin, newMax);
	}

	void UpdateMin(const glm::vec3& vertex)
	{
		Min = glm::vec3{
			(vertex.x < Min.x ? vertex.x : Min.x),
			(vertex.y < Min.y ? vertex.y : Min.y),
			(vertex.z < Min.z ? vertex.z : Min.z) };
	}

	void UpdateMax(const glm::vec3& vertex)
	{
		Max = glm::vec3{
			(vertex.x > Max.x ? vertex.x : Max.x),
			(vertex.y > Max.y ? vertex.y : Max.y),
			(vertex.z > Max.z ? vertex.z : Max.z) };
	}

	void Transform(const glm::mat4& transformation)
	{
		Min = glm::vec3(transformation * glm::vec4(Min, 1.0));
		Max = glm::vec3(transformation * glm::vec4(Max, 1.0));
	}

	AABB Transformed(const glm::mat4& transformation) const
	{
		AABB newAABB = (*this);
		newAABB.Transform(transformation);
		return newAABB;
	}

public:
	glm::vec3 Min = glm::vec3(std::numeric_limits<float>::infinity());
	glm::vec3 Max = glm::vec3(-std::numeric_limits<float>::infinity());

};