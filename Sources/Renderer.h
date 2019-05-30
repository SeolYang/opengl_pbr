#pragma once

class Scene;
class Mesh;
class Model;
class Light;
class Camera;
class Shader;
class Viewport;
class Renderer
{
public:
	Renderer();
	~Renderer();

	bool Init();
	void Render(Scene* scene, Viewport* viewport);

private:
	Shader* m_basicShader;
	Scene* m_targetScene;
	Viewport* m_targetViewport;
	Camera* m_targetCamera;

};