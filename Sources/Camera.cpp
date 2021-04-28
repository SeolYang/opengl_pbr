#include "Camera.h"
#include "Viewport.h"

#include "glm/vec4.hpp"
#include "glm/gtx/transform.hpp"

Camera::Camera(const std::string& name) :
	m_fov(DEFAULT_FOV),
	m_nearPlane(DEFAULT_NEAR_PLANE),
	m_farPlane(DEFAULT_FAR_PLANE),
	m_clearColor(glm::vec3(0.0f)),
	m_lookAt(glm::vec3(0.0f)),
	m_viewport(new Viewport()),
	Object(name)
{
}

Camera::~Camera()
{
	if (m_viewport != nullptr)
	{
		delete m_viewport;
		m_viewport = nullptr;
	}
}

glm::mat4 Camera::GetViewMatrix() const
{
	return glm::lookAt(m_position, m_lookAt, glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::mat4 Camera::GetProjMatrix() const
{
	float width = static_cast<float>(m_viewport->GetWidth());
	float height = static_cast<float>(m_viewport->GetHeight());
	return glm::perspective(glm::radians(m_fov),
		width / height, 
		m_nearPlane, m_farPlane);
}