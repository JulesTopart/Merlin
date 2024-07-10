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

	glm::ivec3 volume_size = glm::ivec3(64,64,64);
	Texture3D_Ptr volume;
	ComputeShader_Ptr noise;
	IsoSurface_Ptr isosurface;

	Scene scene;
	Renderer renderer;

	Shared<PointLight>  light;

	glm::vec3 model_matrix_translation = { 0.8f, 0.2f, 0.3f};
};