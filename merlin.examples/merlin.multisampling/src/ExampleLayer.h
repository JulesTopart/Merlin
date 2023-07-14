#pragma once

#include <Merlin.h>

using namespace Merlin::Graphics;

class ExampleLayer : public Merlin::Layer
{
public:
	ExampleLayer();
	virtual ~ExampleLayer();

	void LoadScene();

	void updateFPS(Timestep ts);

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnEvent(Merlin::Event& event) override;
	virtual void OnUpdate(Merlin::Timestep ts) override;
	virtual void OnImGuiRender() override;
private:
	GLsizei _width = 1080, _height = 720;
	
	Scene scene;
	Renderer renderer;

	Shared<Model>  light;
	Shared<Shader> modelShader;

	std::shared_ptr<FBO> msaa_fbo;
	std::shared_ptr<FBO> fbo;
	std::shared_ptr<ScreenQuadRenderer> screen;

	Shared<Texture> test;

	Shared<Camera> camera;
	Shared<CameraController> cameraController;

	bool msaa = true;
	glm::vec3 model_matrix_translation = { 0.0f, 0.0f, 0.0f};
	float FPS = 0;
	float FPS_sample = 0;
};