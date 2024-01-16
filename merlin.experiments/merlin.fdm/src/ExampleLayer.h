#pragma once

#include <Merlin.h>

using namespace Merlin::Graphics;
using namespace Merlin::Tensor;

#define UNUSED 0
#define SOLID 1
#define FLUID 2
#define GAS 3
#define BOUNDARY 4

struct FluidParticle {
	glm::vec4 initial_position;	// initial position    x0
	glm::vec4 position;			// current position    x
	glm::vec4 new_position;		// predicted position  x*
	glm::vec4 velocity;			// velocity			   u
	glm::vec4 new_velocity;		// velocity			   u*
	glm::vec4 acceleration;		// acceleration		   a
	GLfloat mass;				// mass				   m   (or pseudo mass for boundary particles)
	GLfloat density;			// density			   rho
	GLfloat temperature;		// temperature		   T
	GLfloat temperatureDelta;	// temperature		   T
	GLfloat lambda;				// lagrange multiplier lambda
	GLuint phase;				// phase (liquid, solid...)
	GLuint newIndex;			// sorted indexy
	GLuint binIndex;			// bin index
};


template<>
inline void BufferObject<FluidParticle>::print() {
	Bind();
	Download();

	Console::info("Buffer") << m_name << " : " << Console::endl << "[";
	for (GLuint i = 0; i < ((m_cpuBuffer.size() > 100) ? 100 : m_cpuBuffer.size() - 1); i++) {
		Console::print() << m_cpuBuffer[i].newIndex  << ", ";
	}
	if (m_cpuBuffer.size() > 100) Console::print() << "..., ";
	Console::print() << m_cpuBuffer[m_cpuBuffer.size() - 1].newIndex << "]" << Console::endl << Console::endl;
}

struct Bin {
	GLuint count;
	GLuint sum;
	GLuint csum;
	GLuint index;
};

struct ColorScale {
	GLint maxValue;
	GLint minValue;
};

template<>
inline void BufferObject<Bin>::print() {
	Bind();
	Download();

	Console::info("Buffer") << m_name << " : " << Console::endl << "[";
	for (GLuint i = 0; i < ((m_cpuBuffer.size() > 100) ? 100 : m_cpuBuffer.size() - 1); i++) {
		Console::print() << m_cpuBuffer[i].sum << ", ";
	}
	if (m_cpuBuffer.size() > 100) Console::print() << "..., ";
	Console::print() << m_cpuBuffer[m_cpuBuffer.size() - 1].sum << "]" << Console::endl << Console::endl;
}


//Moving particles
struct Constraint {
	alignas(16) glm::vec4 position;      // Position (x, y, z) and mass (w)
	alignas(16) glm::vec4 velocity;      // Velocity (x, y, z) and inverse density (w)
};

struct Settings {
	
	//Build Volume dimensions
	float bx = 100;//mm
	float by = 35;//mm
	float bz = 60;//mm

	//ex : volume = (100,40,40) & nozzle = 0.8 -> 312.500 particles; nozzle = 0.4 -> 2.500.000 particles)
	//float pDiameter = 1; //mm
	//GLuint pThread = int(bx / (pDiameter)) * int(by / (pDiameter)) * int(bz / (pDiameter)); //Max Number of particles (thread)
	GLuint pThread = 64*64*64; //Max Number of particles (thread)
	GLuint pWkgSize = 512; //Number of thread per workgroup
	GLuint pWkgCount = (pThread + pWkgSize - 1) / pWkgSize; //Total number of workgroup needed

	GLuint bRes = 42; //Bed width is divided bRes times
	float bWidth = max(bx, max(by, bz)) / float(bRes); //Width of a single bin in mm
	GLuint bThread = int(bx / (bWidth)) * int(by / (bWidth)) * int(bz / (bWidth)); //Total number of bin (thread)
	GLuint blockSize = floor(log2f(bThread));
	GLuint blocks = (bThread + blockSize - 1) / blockSize;

	GLuint bWkgSize = 256; //Number of thread per workgroup
	GLuint bWkgCount = (blocks + bWkgSize - 1) / bWkgSize; //Total number of workgroup needed

	// --- SPH ---
	// SPH Parameters
	float particleRadius = 1.2; // mm
	float H = 1.7; // Kernel radius mm
	float REST_DENSITY = 1.0; // g/mm3 Metled plastic
	float particleMass = 1.0;//g Mass
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

	void InitGraphics();
	void InitPhysics();
	void ResetSimulation();
	void SetColorGradient();
	void UpdateBufferSettings();
	
	void NeigborSearch();
	void Simulate(Merlin::Timestep ts);

	void updateFPS(Merlin::Timestep ts);

private:
	glm::uvec3 getBinCoord(glm::vec3 position);
	GLuint getBinIndexFromCoord(glm::uvec3 coord);
	GLuint getBinIndex(glm::vec3 position);
	glm::uvec3 getBinCoordFromIndex(GLuint index);

	GLsizei _width = 1080, _height = 720;

	//Simulation
	ComputeShader_Ptr init;
	StagedComputeShader_Ptr solver;
	StagedComputeShader_Ptr prefixSum;

	SSBO_Ptr<Bin> binBuffer; //Particle buffer
	SSBO_Ptr<Constraint> constraintBuffer; //Index buffer
	SSBO_Ptr<FluidParticle> particleBuffer; //Particle buffer
	SSBO_Ptr<ColorScale> colorScaleBuffer; //Particle buffer

	Shader_Ptr modelShader;
	Shader_Ptr particleShader;
	Shader_Ptr binShader;

	ParticleSystem_Ptr<FluidParticle> particleSystem;
	ParticleSystem_Ptr<FluidParticle> binSystem;

	Renderer renderer;
	Scene scene;
	TransformObject_Ptr origin;
	Model_Ptr  light;

	//Heat Map
	SSBO_Ptr<glm::vec4> heatMap;
	int colorCount;

	//Camera
	Camera_Ptr camera;
	CameraController_Ptr cameraController;

	Settings settings;

	//Simulation
	GLuint numParticles = 0;
	GLuint numBoundaryParticles = 0;
	glm::vec3 model_matrix_translation = { 0.0f, 0.0f, 0.0f };
	int solver_iteration = 15;

	float elapsedTime = 0;
	bool paused = true;
	bool integrate = true;
	float sim_speed = 1;
	float camera_speed = 1;
	float FPS = 0;
	float FPS_sample = 0;
};