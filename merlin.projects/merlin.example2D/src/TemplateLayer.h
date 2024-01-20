#pragma once

#include <Merlin.h>
using namespace Merlin::Graphics;

class TemplateLayer : public Merlin::Layer
{
public:
	TemplateLayer();
	virtual ~TemplateLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnEvent(Merlin::Event& event) override;
	virtual void OnUpdate(Merlin::Timestep ts) override;
	virtual void OnImGuiRender() override;
private:

	Shared<Camera> camera;

	Scene scene;
	Renderer renderer;

	Shared<Model>  light;
	Shared<SkyBox> sky;
	Shared<Shader> modelShader;

	glm::vec3 model_matrix_translation = { 0.8f, 0.2f, 0.3f};
};