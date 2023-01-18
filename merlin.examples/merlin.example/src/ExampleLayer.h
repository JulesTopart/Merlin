#pragma once

#include <Merlin.h>
using namespace Merlin::Scene;
using namespace Merlin::Renderer;

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
	Shared<Shader> modelShader;
	Shared<Shader> axisShader;

	Shared<Mesh> axis;
	Shared<Mesh> model;

	Shared<Camera> camera;
	Shared<CameraController> cameraController;

	glm::vec3 model_matrix_translation = { 0.8f, 0.2f, 0.3f};
};