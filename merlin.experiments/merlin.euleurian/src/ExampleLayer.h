#pragma once

#include <Merlin.h>

using namespace Merlin::Graphics;
using namespace Merlin::Tensor;

struct FieldProperty{
	float v[3];	//Velocity
	float p;	//Pressure

	float d;	//Divergence
	float m;	//mass
	float t;	//Temperature
	float s;	//Grid element type (solid, liquid...)
};

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
	std::shared_ptr<ComputeShader> physics;

	Renderer renderer;
	Scene scene;

	Shared<Model>  light;
	Shared<Shader> modelShader;

	//Heat Map
	SSBO_Ptr<Color> heatMap;
	int colorCount;

	//Buffers
	SSBO_Ptr<FieldProperty> grid;
	SSBO_Ptr<FieldProperty> newGrid;

	deprecated_ParticleSystem_Ptr streamlines;

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