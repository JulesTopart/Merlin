#pragma once

#include <Merlin.h>

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
	std::shared_ptr<Merlin::Renderer::Shader> modelShader;
	std::shared_ptr<Merlin::Renderer::Shader> axisShader;

	std::shared_ptr<Merlin::Renderer::Mesh> axis;
	std::shared_ptr<Merlin::Renderer::Mesh> model;

	Merlin::Scene::CameraController cameraController;

	glm::vec3 model_matrix_translation = { 0.8f, 0.2f, 0.3f};
};