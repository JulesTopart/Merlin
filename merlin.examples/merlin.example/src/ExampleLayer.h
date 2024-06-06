#pragma once

#include <Merlin.h>
using namespace Merlin;

class ExampleLayer : public Layer3D
{
public:
	ExampleLayer();
	virtual ~ExampleLayer() {};

	virtual void onAttach() override;
	virtual void onDetach() override;
	virtual void onEvent(Event& event) override;
	virtual void onUpdate(Timestep ts) override;
	virtual void onImGuiRender() override;
private:

	Scene scene;
	Renderer renderer;

	Shared<PointLight>  light;

	glm::vec3 model_matrix_translation = { 0.8f, 0.2f, 0.3f};
};