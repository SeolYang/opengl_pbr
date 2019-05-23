#pragma once
#include <vector>
#include <string>
#include "glm/vec3.hpp"

struct ApplicationSettings
{
	unsigned int width;
	unsigned int height;
	std::string title;
};

struct SceneSettings
{
	struct Camera {
		float nearPlane;
		float farPlane;
		float fov;
		glm::vec3 position;
		glm::vec3 rotation;
	};
	std::vector<Camera> cameras;

	struct Light {
		glm::vec3 position;
		glm::vec3 direction;
		glm::vec3 radiance;
	};
	std::vector<Light> lights;
};