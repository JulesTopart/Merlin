#pragma once

#include <Merlin.h>

using namespace Merlin::Graphics;
using namespace Merlin::Tensor;


// Particle thermo-visco elastic properties
struct TVEParticle {
	float position[3];      // position (x, y, z)
	float mass;				// mass(w)

	float velocity[3];      // velocity (x, y, z) and density (w)
	float density;			// density g/m3
	
	float stressTensorX[3]; // stress tensor (xx, xy, xz)
	float temperature;		// temperature (K)
	
	float stressTensorY[3]; // stress tensor (yx, yy, yz, 0)
	float pressure;			// pressure (Pa)

	float stressTensorZ[3]; // stress tensor (zx, zy, zz, 0)
	float strain;			// strain (Pa)
};

//Moving particles
struct SimpleParticle {
	alignas(16) glm::vec4 position;      // Position (x, y, z) and mass (w)
	alignas(16) glm::vec4 velocity;      // Velocity (x, y, z) and inverse density (w)
};


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

	//Mesh instancing
	std::shared_ptr<ComputeShader> init;
	std::shared_ptr<ComputeShader> physics;

	Shared<ParticleSystem> particleSystem;

	Renderer renderer;
	Scene scene;

	Shared<Model>  light;
	Shared<Shader> modelShader;

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