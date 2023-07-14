#pragma once

#include <Merlin.h>

using namespace Merlin::Graphics;
using namespace Merlin::Tensor;

struct Color {
	alignas(16) glm::vec3 color;
	float value;
};

class ExampleLayer : public Merlin::Layer
{
public:
	ExampleLayer();
	virtual ~ExampleLayer();

	void InitGraphics();
	void InitPhysics();
	void SetColorGradient();
	void Simulate();
	void BindImages();
	void SyncTextureUnits(ShaderBase& sh);


	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnEvent(Merlin::Event& event) override;
	virtual void OnUpdate(Merlin::Timestep ts) override;
	virtual void OnImGuiRender() override;

	void updateFPS(Merlin::Timestep ts);

private:
	GLsizei _width = 1080, _height = 720;

	//Mesh instancing
	std::shared_ptr<ComputeShader> init;
	std::shared_ptr<ComputeShader> pre;
	std::shared_ptr<ComputeShader> post;

	std::shared_ptr<ComputeShader> project;
	std::shared_ptr<ComputeShader> advect;

	Shared<ScreenQuadRenderer> renderer;

	//Heat Map
	Shared<SSBO> heatMap;
	int colorCount;

	//Buffers
	Shared<Texture> velocity;
	Shared<Texture> flow ;
	Shared<Texture> new_velocity;
	Shared<Texture> phase;
	Shared<Texture> pressure;

	//Camera
	Shared<Camera> camera;
	Shared<CameraController> cameraController;

	glm::vec3 model_matrix_translation = { 0.0f, 0.0f, 0.0f };

	bool paused = true;
	float sim_speed = 1;
	float camera_speed = 1;
	float FPS = 0;
	float FPS_sample = 0;
};