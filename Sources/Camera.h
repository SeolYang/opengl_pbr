#pragma once
#include "Object.h"

#include "glm/vec3.hpp"

constexpr float DEFAULT_FOV = 45.0f;
constexpr float DEFAULT_NEAR_PLANE = 0.01f;
constexpr float DEFAULT_FAR_PLANE = 1.0e+03f;

class Viewport;
class Camera : public Object
{
public:
	Camera(const std::string& name);
	~Camera();

	void SetNearPlane(float nearPlane) { m_nearPlane = nearPlane; }
	float GetNearPlane() const { return m_nearPlane; }

	void SetFarPlane(float farPlane) { m_farPlane = farPlane; }
	float GetFarPlane() const { return m_farPlane; }

	void SetFOV(float fov) { m_fov = fov; }
	float GetFOV() const { return m_fov; }

	void SetClearColor(glm::vec3 color) { m_clearColor = color; }
	glm::vec3 GetClearColor() const { return m_clearColor; }

	Viewport* GetViewport() const { return m_viewport; }

	glm::mat4 GetViewMatrix() const;
	glm::mat4 GetProjMatrix() const;

	void SetLookAt(const glm::vec3& newLookPos) { m_lookAt = newLookPos; }
	glm::vec3 GetLookAt() const { return m_lookAt; }

private:
	float m_fov;
	float m_nearPlane;
	float m_farPlane;

	glm::vec3 m_lookAt;
	glm::vec3 m_clearColor;

	Viewport* m_viewport;

};