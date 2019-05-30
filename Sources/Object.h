#pragma once
#include <string>

#include "glm/vec3.hpp"
#include "glm/matrix.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Object
{
public:
	Object(const std::string& name) :
		m_name(name)
	{
	}

	std::string GetName() const { return m_name; }

	glm::vec3 GetPosition() const { return m_position; }
	void SetPosition(glm::vec3 position) { m_position = position; }

	glm::quat GetRotation() const { return m_rotation; }
	void SetRotation(glm::quat rotation) { m_rotation = rotation; }

	glm::vec3 GetScale() const { return m_scale; }
	void SetScale(glm::vec3 scale) { m_scale = scale; }

	glm::mat4 GetWorldMatrix() const 
	{
		glm::mat4 res = glm::mat4(1.0f);
		res = glm::scale(res, m_scale);
		res = glm::toMat4(m_rotation) * res;
		res = glm::translate(res, m_position);
		return res;
	}

protected:
	glm::vec3 m_position;
	glm::quat m_rotation;
	glm::vec3 m_scale;

private:
	std::string m_name;

};