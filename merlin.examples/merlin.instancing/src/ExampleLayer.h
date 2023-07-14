#pragma once

#include <Merlin.h>

using namespace Merlin::Graphics;
using namespace Merlin::Tensor;


// Particle thermo-visco elastic properties
struct TVEParticle {
	float position[3];
	float mass;
	float velocity[3];
	float density;
	float stressTensorX[3];      
	float temperature;
	float stressTensorY[3];     
	float pressure;
	float stressTensorZ[3];
	float strain;
};

//Moving particles
struct SimpleParticle {
	alignas(16) glm::vec4 position;      // Position (x, y, z) and mass (w)
	alignas(16) glm::vec4 velocity;      // Velocity (x, y, z) and density (w)
};


class ExampleLayer : public Merlin::Layer
{
public:
	ExampleLayer();
	virtual ~ExampleLayer();

	void InitGraphics();
	void InitPhysics();

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

	Shared<ParticleSystem> particleSystem;

	Renderer renderer;
	Scene scene;

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