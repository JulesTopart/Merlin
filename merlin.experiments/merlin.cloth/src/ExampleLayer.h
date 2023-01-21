#pragma once

#include <Merlin.h>

using namespace Merlin::Renderer;
using namespace Merlin::Tensor;

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
	GLsizei _width = 1080, _height = 720;
	
	//Mesh rendering
	Shared<Shader> modelShader;
	Shared<Shader> axisShader;
	Shared<Mesh> axis;
	Shared<Mesh> model;

	//Mesh instancing
	Shared<ParticleSystem> particleSystem;
	Shared<Shader> particleShader;
	Shared<ComputeShader> init;

	//Camera
	Merlin::Scene::CameraController cameraController;

	//Ui
	bool paused = true;
	glm::vec3 model_matrix_translation = { 0.0f, 0.0f, 0.0f };
	float sim_speed = 1;
	float camera_speed = 1;
	float FPS = 0;
	float FPS_sample = 0;
};