#include "Camera.h"
#include "glm/vec4.hpp"
#include "glm/gtx/transform.hpp"

Camera::Camera(const std::string& name) :
	m_fov(DEFAULT_FOV),
	m_nearPlane(DEFAULT_NEAR_PLANE),
	m_farPlane(DEFAULT_FAR_PLANE),
	m_clearColor(glm::vec3(0.0f)),
	Object(name)
{
}

glm::mat4 Camera::GetViewMatrix() const
{
	glm::vec4 right{ 1.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 up{ 0.0f, 1.0f, 0.0f, 0.0f };
	glm::vec4 forward{ 0.0f, 0.0f, -1.0f, 0.0f };

	glm::vec3 position{ m_position };

	right = glm::normalize(glm::rotate(m_rotation, right));
	up = glm::normalize(glm::rotate(m_rotation, up));
	forward = glm::normalize(glm::rotate(m_rotation, forward));

	glm::mat4 invTranslation= glm::translate(-position);
	glm::mat4 invRotation{ right, up, forward, glm::vec4() };
	invRotation = glm::transpose(invRotation);

	return (invRotation * invTranslation);
}

glm::mat4 Camera::GetProjMatrix(float width, float height) const
{
	return glm::perspectiveFov(m_fov, 
		width, height,
		m_nearPlane, m_farPlane);
}