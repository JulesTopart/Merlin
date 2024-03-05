#pragma once

#include <Merlin.h>
using namespace Merlin;

class ExampleLayer : public Layer
{
public:
	ExampleLayer();
	virtual ~ExampleLayer();

	virtual void onAttach() override;
	virtual void onDetach() override;
	virtual void onEvent(Event& event) override;
	virtual void onUpdate(Timestep ts) override;
	virtual void onImGuiRender() override;
private:

	Shared<Camera> camera;
	Shared<CameraController> cameraController;

	Scene scene;
	Renderer renderer;

	Shared<Model>  light;
	Shared<SkyBox> sky;
	Shared<Shader> modelShader;

	glm::vec3 model_matrix_translation = { 0.8f, 0.2f, 0.3f};
};