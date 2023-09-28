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
	GLuint newIndex;		// sorted index
	GLuint binIndex;		// bin index
};


template<>
inline void BufferObject<FluidParticle>::print() {
	Bind();
	Sync();

	Console::info("Buffer") << m_name << " : " << Console::endl << "[";
	for (GLuint i = 0; i < ((m_cpuBuffer.size() > 100) ? 100 : m_cpuBuffer.size() - 1); i++) {
		Console::print() << m_cpuBuffer[i].binIndex << ", ";
	}
	if (m_cpuBuffer.size() > 100) Console::print() << "..., ";
	Console::print() << m_cpuBuffer[m_cpuBuffer.size() - 1].binIndex << "]" << Console::endl << Console::endl;
}


struct Bin {
	GLuint count;
	GLuint sum;
	GLuint index;
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
	GLuint pWkgSize = 256; //Number of thread per workgroup
	GLuint pWkgCount = (pThread + pWkgSize - 1) / pWkgSize; //Total number of workgroup needed

	GLuint bRes = 64; //Bed width is divided bRes times
	float bWidth = max(bx, max(by, bz)) / float(bRes); //Width of a single bin in mm
	GLuint bThread = int(bx / (bWidth)) * int(by / (bWidth)) * int(bz / (bWidth)); //Total number of bin (thread)
	GLuint bWkgSize = bRes; //Number of thread per workgroup
	GLuint bWkgCount = (bThread + bWkgSize - 1) / bWkgSize; //Total number of workgroup needed

	const GLuint blockSize = floor(log2f(pThread));
	const GLuint blocks = (pThread + blockSize - 1) / blockSize;

	const GLuint bwgSize = 256; //WorkGroup size
	const GLuint bwgCount = (blocks + bwgSize - 1) / bwgSize; //WorkGroup size
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
	ComputeShader_Ptr init;
	StagedComputeShader_Ptr solver;
	StagedComputeShader_Ptr prefixSum;

	SSBO_Ptr<Bin> binBuffer; //Particle buffer
	SSBO_Ptr<Constraint> constraintBuffer; //Index buffer
	SSBO_Ptr<FluidParticle> particleBuffer; //Particle buffer

	//Shared<UBO> simParameters;

	Shader_Ptr modelShader;
	Shader_Ptr particleShader;
	Shader_Ptr binShader;

	ParticleSystem_Ptr<FluidParticle> particleSystem;
	ParticleSystem_Ptr<FluidParticle> binSystem;

	Renderer renderer;
	Scene scene;

	Model_Ptr  light;
	Model_Ptr  nozzleMdl;
	TransformObject_Ptr  nozzle;

	//Heat Map
	SSBO_Ptr<Color> heatMap;
	int colorCount;

	//Camera
	Camera_Ptr camera;
	CameraController_Ptr cameraController;

	//Simulation
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