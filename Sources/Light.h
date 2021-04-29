#pragma once
#include "Object.h"
#include "glm/vec3.hpp"

class Light : public Object
{
public:
	Light(const std::string& name);

	glm::vec3 GetIntensity() const { return m_intensity; }
	void SetIntensity(const glm::vec3& intensity) { m_bIsDirty = true; m_intensity = intensity; }

private:
	glm::vec3 m_intensity = glm::vec3(1.0f);

};