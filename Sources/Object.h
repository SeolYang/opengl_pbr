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
		m_bIsActive(true),
		m_position(glm::vec3(0.0f)),
		m_scale(glm::vec3(1.0f)),
		m_rotation(glm::quat()),
		m_name(name)
	{
	}
	~Object() = default;

	std::string GetName() const { return m_name; }

	glm::vec3 GetPosition() const { return m_position; }
	void SetPosition(glm::vec3 position) { m_bIsDirty = true; m_position = position; }

	glm::quat GetRotation() const { return m_rotation; }
	void SetRotation(glm::quat rotation) { m_bIsDirty = true; m_rotation = rotation; }

	glm::vec3 GetScale() const { return m_scale; }
	void SetScale(glm::vec3 scale) { m_bIsDirty = true; m_scale = scale; }

	bool IsActivated() const { return m_bIsActive; }
	void SetActive(bool bIsActive) { m_bIsDirty = true; m_bIsActive = bIsActive; }

	glm::mat4 GetWorldMatrix() const
	{
		glm::mat4 identity = glm::mat4(1.0f);
		auto scale = glm::scale(identity, m_scale);
		auto rotate = glm::toMat4(m_rotation);
		auto translate = glm::translate(identity, m_position);
		auto res = translate * rotate * scale;
		return res;
	}

	glm::vec3 Forward() const { return glm::rotate(m_rotation, glm::vec3(0.0f, 0.0f, 1.0f)); }

	bool IsDirty() const { return m_bIsDirty; }
	void ResolveDirty() { m_bIsDirty = false; }

protected:
	glm::vec3 m_position;
	glm::quat m_rotation;
	glm::vec3 m_scale;

	bool m_bIsDirty = true;

private:
	std::string m_name;
	bool m_bIsActive;

};