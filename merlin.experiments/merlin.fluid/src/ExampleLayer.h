#pragma once

#include <Merlin.h>

using namespace Merlin::Renderer;
using namespace Merlin::Tensor;
using namespace Merlin::Scene;

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

	void updateFPS(Merlin::Timestep ts);

private:

	//Mesh rendering
	Shared<Shader> axisShader;
	Shared<Primitive> axis;

	//Mesh instancing
	Shared<ParticleSystem> particleSystem;
	Shared<Shader> particleShader;
	Shared<ComputeShader> init;
	Shared<ComputeShader> fluid1;
	Shared<ComputeShader> fluid2;

	//Camera
	
	Shared<Camera> camera;
	Shared<CameraController> cameraController;

	//Ui
	bool paused = true;
	glm::vec3 model_matrix_translation = { 0.0f, 0.0f, 0.0f };
	glm::vec3 model_matrix_rotation = { 0.0f, 0.0f, 0.0f };
	float sim_speed = 1.0f;
	float camera_speed = 1;
	float FPS = 0;
	float FPS_sample = 0;
};