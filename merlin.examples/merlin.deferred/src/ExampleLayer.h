#pragma once

#include <Merlin.h>
using namespace Merlin::Graphics;

class ExampleLayer : public Merlin::Layer
{
public:
	ExampleLayer();
	virtual ~ExampleLayer();

	void LoadScene();

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnEvent(Merlin::Event& event) override;
	virtual void OnUpdate(Merlin::Timestep ts) override;
	virtual void OnImGuiRender() override;
private:

	GLsizei _width = 1080, _height = 720;

	Shared<Camera> camera;
	Shared<CameraController> cameraController;

	Scene scene;
	Renderer renderer;

	Shared<Model>  light;
	Shared<Shader> modelShader;


	std::shared_ptr<FBO> fbo;
	std::shared_ptr<ScreenQuadRenderer> screen;

	int bufferID = 0;

	glm::vec3 model_matrix_translation = { 0.8f, 0.2f, 0.3f};
};