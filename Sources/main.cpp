#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tinygltf/tiny_gltf.h"
#include "TestApp.h"

int main()
{
	Application* app = new TestApp("Test", 1280, 720);

	int res = app->Run();
	delete app;
	app = nullptr;

	return res;
}