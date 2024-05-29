#pragma once

#include <Merlin.h>
using namespace Merlin;

class AppLayer : public Layer
{
public:
	AppLayer();
	virtual ~AppLayer();

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

	Shared<PointLight>  light;

	glm::vec3 model_matrix_translation = { 0.8f, 0.2f, 0.3f};
};