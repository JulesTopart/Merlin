#pragma once

#include <Merlin.h>
using namespace Merlin;

class TemplateLayer : public Layer2D
{
public:
	TemplateLayer();
	virtual ~TemplateLayer();

	virtual void onAttach() override;
	virtual void onDetach() override;
	virtual void onEvent(Event& event) override;
	virtual void onUpdate(Timestep ts) override;
	virtual void onImGuiRender() override;
private:
	Scene scene;
	Renderer renderer;

	Shared<Light>  light;

	glm::vec3 model_matrix_translation = { 0.8f, 0.2f, 0.3f};
};