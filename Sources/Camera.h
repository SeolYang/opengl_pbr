#pragma once
#include "Object.h"
#include "glm/vec3.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/matrix.hpp"

constexpr float DEFAULT_FOV = 45.0f;
constexpr float DEFAULT_NEAR_PLANE = 1.0e-01;
constexpr float DEFAULT_FAR_PLANE = 1.0e+03;

class Camera : public Object
{
public:
	Camera(const std::string& name);

	void SetNearPlane(float nearPlane) { m_nearPlane = nearPlane; }
	float GetNearPlane() const { return m_nearPlane; }

	void SetFarPlane(float farPlane) { m_farPlane = farPlane; }
	float GetFarPlane() const { return m_farPlane; }

	void SetPosition(const glm::vec3& position) { m_position = position; }
	glm::vec3 GetPosition() const { return m_position; }

	void SetRotation(const glm::quat& rotation) { m_rotation = rotation; }
	glm::quat GetRotation() const { return m_rotation; }

	void SetFOV(float fov) { m_fov = fov; }
	float GetFOV() const { return m_fov; }

	glm::mat4 GetViewMatrix() const;
	glm::mat4 GetProjMatrix(float width, float height) const;

private:
	float m_fov;
	float m_nearPlane;
	float m_farPlane;
	glm::vec3 m_position;
	glm::quat m_rotation;

};