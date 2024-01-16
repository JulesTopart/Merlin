#pragma once

#include <Merlin.h>
#include <glm/glm.hpp>

using namespace Merlin::Graphics;
using namespace Merlin::Tensor;

#define UNUSED 0
#define SOLID 1
#define FLUID 2
#define GAS 3
#define GRANULAR 4
#define BOUNDARY 5

struct FluidParticle {
	alignas(16) glm::vec4 initial_position;	// initial position    x0
	alignas(16) glm::vec4 pposition;			// predicted position  x~
	alignas(16) glm::vec4 position;			// current position    x
	alignas(16) glm::vec4 new_position;		// predicted position  x+1
	alignas(16) glm::vec4 velocity;			// velocity			   u
	alignas(16) glm::vec4 new_velocity;		// velocity			   u*
	alignas(16) glm::vec4 acceleration;		// acceleration		   a
	GLfloat mass;				// mass				   m   (or pseudo mass for boundary particles)
	GLfloat density;			// density			   rho
	GLfloat pressure;			// pressure			   P
	GLfloat temperature;		// temperature		   T
	GLfloat new_temperature;	// new temperature	   T+1
	GLfloat lambda;			// lagrange multiplier lambda
	GLfloat dummyA;			//available
	GLfloat dummyB;			//available
	GLfloat dummyC;			//available
	GLuint phase;				// phase (liquid, solid...)
	GLuint newIndex;			// sorted index
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

/*
struct ElasticConstraint {
	GLuint a; //Particle A
	GLuint b; //Particle B
	GLfloat restLength;
};

struct ThermoElasticConstraint {
	GLuint a; //Particle A
	GLuint b; //Particle B
	GLfloat restLength;			//spring restlenght
	GLfloat thermalDeformation; //thermal induced deformation
};

struct ElastoPlasticConstraint {
	GLuint a; //Particle A
	GLuint b; //Particle B
	GLfloat restLength;
	GLfloat plasticDeformation;
};
*/

struct DistanceContraint {
	GLuint nodes[8]; //Particle A
	GLfloat compliance;
	GLfloat restLength;
	GLfloat thermalDeformation; //thermal induced deformation
	GLfloat plasticDeformation;
};



struct Settings {
	
	//Build Volume dimensions
	glm::vec3 bb = glm::vec3(30, 30, 100);
	float bx = bb.x;//mm 120
	float by = bb.y;//mm
	float bz = bb.z;//mm 40
	
	GLuint bRes = 128; //Bed width is divided bRes times (old 42)

	GLuint maxNNS = 32;

	//ex : volume = (100,40,40) & nozzle = 0.8 -> 312.500 particles; nozzle = 0.4 -> 2.500.000 particles)
	//float pDiameter = 1; //mm
	//GLuint pThread = int(bx / (pDiameter)) * int(by / (pDiameter)) * int(bz / (pDiameter)); //Max Number of particles (thread)
	GLuint pThread = 1000000; //Max Number of particles (thread) (10 milion)
	GLuint pWkgSize = 512; //Number of thread per workgroup
	GLuint pWkgCount = (pThread + pWkgSize - 1) / pWkgSize; //Total number of workgroup needed

	
	float bWidth = std::max(bx, std::max(by, bz)) / float(bRes); //Width of a single bin in mm
	GLuint bThread = int(bx / (bWidth)) * int(by / (bWidth)) * int(bz / (bWidth)); //Total number of bin (thread)
	GLuint blockSize = floor(log2f(bThread));
	GLuint blocks = (bThread + blockSize - 1) / blockSize;

	GLuint bWkgSize = 512; //Number of thread per workgroup
	GLuint bWkgCount = (blocks + bWkgSize - 1) / bWkgSize; //Total number of workgroup needed

	// --- SPH ---
	// SPH Parameters
	float particleRadius = 0.2; // mm
	float H = 3 * particleRadius; // Kernel radius mm
	float REST_DENSITY = 1.0; // g/mm3 Metled plastic
	float particleMass = 1.0;//g Mass
	float timeStep = 0.016;//g Mass

	int solver_substep = 30;
	int solver_iteration = 1;
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
	void UpdateBufferSettings();
	
	void NeigborSearch();
	void Simulate(Merlin::Timestep ts);

	void updateFPS(Merlin::Timestep ts);

private:
	GLsizei _width = 1080, _height = 720;

	//Simulation
	StagedComputeShader_Ptr solver;
	StagedComputeShader_Ptr prefixSum;

	SSBO_Ptr<Bin> binBuffer; //Particle buffer
	SSBO_Ptr<FluidParticle> particleBuffer; //Particle buffer
	SSBO_Ptr<DistanceContraint> constraintBuffer; //Particle buffer

	Shader_Ptr modelShader;
	Shader_Ptr particleShader;
	Shader_Ptr binShader;

	deprecated_ParticleSystem_Ptr particleSystem;
	deprecated_ParticleSystem_Ptr binSystem;

	Renderer renderer;
	Scene scene;
	TransformObject_Ptr origin;
	Model_Ptr  light;

	//Camera
	Camera_Ptr camera;
	CameraController_Ptr cameraController;

	Settings settings;

	//Simulation
	GLuint numParticles = 0;
	GLuint numConstraint = 0;
	GLuint numBoundaryParticles = 0;
	glm::vec3 model_matrix_translation = { 0.0f, 0.0f, 0.0f };



	

	double nns_time = 0;
	double solver_substep_time = 0;
	double solver_total_time = 0;
	double render_time = 0; double render_start_time = 0;
	double total_time = 0; double total_start_time = 0;

	float elapsedTime = 0;
	bool paused = true;
	bool integrate = true;
	float sim_speed = 1;
	float camera_speed = 1;
	float FPS = 0;
	float FPS_sample = 0;
};