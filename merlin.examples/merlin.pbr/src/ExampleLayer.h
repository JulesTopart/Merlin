#pragma once

#include <Merlin.h>
using namespace Merlin::Graphics;

class ExampleLayer : public Merlin::Layer
{
public:
	ExampleLayer();
	virtual ~ExampleLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnEvent(Merlin::Event& event) override;
	virtual void OnUpdate(Merlin::Timestep ts) override;
	virtual void OnImGuiRender() override;
private:

	
	Scope<Renderer> renderer_ptr;
	Scope<Scene> scene_ptr;

	Shared<Camera> camera;
	Shared<CameraController> cameraController;

	Shared<Model>  light;
	Shared<SkyBox> sky;
	Shared<Shader> modelShader;

	glm::vec3 model_matrix_translation = { 0.8f, 0.2f, 0.3f};
};