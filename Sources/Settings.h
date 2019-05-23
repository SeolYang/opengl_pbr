#pragma once
#include <vector>
#include "glm/"

struct ApplicationSettings
{
	unsigned int width;
	unsigned int height;
};

struct SceneSettings
{
	struct CameraSettings {
		float nearPlane;
		float farPlane;
		float fov;
	};
	std::vector<CameraSettings> cameras;
};