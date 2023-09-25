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
	float a;				// empty
	float b;				// empty
	float c;				// empty
	GLuint binIndex;		// bin index

};

struct Bin {
	GLuint count;
	GLuint startIndex;
};

//Moving particles
struct Constraint {
	alignas(16) glm::vec4 position;      // Position (x, y, z) and mass (w)
	alignas(16) glm::vec4 velocity;      // Velocity (x, y, z) and inverse density (w)
};

const struct Settings {
	float scale = 0.035 / (4.0);

	//Build Volume dimensions
	float bx = 100;//mm
	float by = 40;//mm
	float bz = 40;//mm

	//ex : volume = (100,40,40) & nozzle = 0.8 -> 312.500 particles; nozzle = 0.4 -> 2.500.000 particles)
	float pDiameter = 0.4; //mm
	GLuint pThread = int(bx / (pDiameter)) * int(by / (pDiameter)) * int(bz / (pDiameter)); //Max Number of particles (thread)
	GLuint pWkgSize = 128; //Number of thread per workgroup
	GLuint pWkgCount = (pThread + pWkgSize - 1) / pWkgSize; //Total number of workgroup needed

	GLuint bRes = 64; //Bed width is divided bRes times
	float bWidth = max(bx, max(by, bz)) / float(bRes); //Width of a single bin in mm
	GLuint bThread = int(bx / (bWidth)) * int(by / (bWidth)) * int(bz / (bWidth)); //Total number of bin (thread)
	GLuint bWkgSize = bRes; //Number of thread per workgroup
	GLuint bWkgCount = (bThread + bWkgSize - 1) / bWkgSize; //Total number of workgroup needed
}settings;

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
	Shared<ComputeShader> prefixSum;

	
	SSBO<Bin> binBuffer; //Particle buffer
	std::vector<Bin> binCPUBuffer; //Particle buffer

	SSBO<Constraint> constraintBuffer; //Index buffer
	std::vector<Constraint> constraintCPUBuffer; //Particle buffer

	SSBO<FluidParticle> particleBuffer; //Particle buffer
	std::vector<FluidParticle> particleCPUBuffer; //Particle buffer


	//Shared<UBO> simParameters;

	Shared<Shader> modelShader;
	Shared<Shader> particleShader;
	Shared<Shader> binShader;

	Shared<ParticleSystem> particleSystem;
	Shared<ParticleSystem> binSystem;

	Renderer renderer;
	Scene scene;

	Shared<Model>  light;
	Shared<Model>  nozzleMdl;
	Shared<TransformObject>  nozzle;

	//Heat Map
	SSBO<Color> heatMap;
	int colorCount;

	//Camera
	Shared<Camera> camera;
	Shared<CameraController> cameraController;

	GLuint numParticles = 0;

	glm::vec3 model_matrix_translation = { 0.0f, 0.0f, 0.0f };
	int solver_iteration = 15;

	int sim = 0;

	bool paused = true;
	float sim_speed = 1;
	float camera_speed = 1;
	float FPS = 0;
	float FPS_sample = 0;
};