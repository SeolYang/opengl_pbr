#pragma once
#include "Object.h"
#include "glm/vec3.hpp"

class Light : public Object
{
public:
	Light(const std::string& name);

	glm::vec3 GetRadiance() const { return m_radiance; }
	void SetRadiance(const glm::vec3& radiance) { m_radiance = radiance; }

private:
	glm::vec3 m_radiance;

};