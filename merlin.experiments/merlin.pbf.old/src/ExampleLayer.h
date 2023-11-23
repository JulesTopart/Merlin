#pragma once

#include <Merlin.h>

using namespace Merlin::Graphics;
using namespace Merlin::Tensor;

//Heatmap color
struct Color {
	alignas(16) glm::vec3 color;
	float value;
};


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

struct FluidParticle {
	float position[3];      // current position   (x, y, z, 0)
	float density;
	float new_position[3];  // predicted position (x, y, z, 0);
	float lambda;
	float velocity[3];		// velocity			  (vx, vy, vz, 0);
	float temperature;		// padding
};

struct Bin {
	GLuint count;
	GLuint indices[32]; //average is 16
};


const GLuint thread = 4;
const GLsizei binCount = 128 * 128 * 128;
const GLsizei maxParticlesCount = 32 * 32 * 32;


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


	void InitGraphics();
	void InitPhysics();
	void ResetSimulation();
	void SetColorGradient();
	void Simulate(Merlin::Timestep ts);

	void updateFPS(Merlin::Timestep ts);

private:
	GLsizei _width = 1080, _height = 720;

	//Simulation
	Shared<ComputeShader> init;
	Shared<ComputeShader> solver;

	Shared<SSBO> binBuffer; //Particle buffer
	Shared<SSBO> particleBuffer; //Particle buffer
	//Shared<SSBO> sortedparticleBuffer; //Particle buffer

	Shared<UBO> simParameters;

	Shared<Shader> modelShader;
	Shared<Shader> particleShader;

	Shared<ParticleSystem> particleSystem;

	Renderer renderer;
	Scene scene;

	Shared<Model>  light;

	//Heat Map
	Shared<SSBO> heatMap;
	int colorCount;

	//Camera
	Shared<Camera> camera;
	Shared<CameraController> cameraController;

	GLuint numParticles = 0;

	glm::vec3 model_matrix_translation = { 0.0f, 0.0f, 0.0f };
	int solver_iteration = 40;

	int sim = 0;

	bool paused = true;
	float sim_speed = 1;
	float camera_speed = 1;
	float FPS = 0;
	float FPS_sample = 0;
};