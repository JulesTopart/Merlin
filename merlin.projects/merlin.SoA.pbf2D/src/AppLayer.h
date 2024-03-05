#pragma once
#include "settings.h"
using namespace Merlin;

#define UNUSED 0
#define SOLID 1
#define FLUID 2
#define GAS 3
#define GRANULAR 4
#define BOUNDARY 5

class AppLayer : public Merlin::Layer{
public:
	AppLayer();
	virtual ~AppLayer();

	void onAttach() override;
	void onDetach() override;
	void onEvent(Merlin::Event& event) override;
	void onUpdate(Merlin::Timestep ts) override;
	void onImGuiRender() override;

	void InitGraphics();
	void InitPhysics();
	void ResetSimulation();

	void SyncUniforms();
	void ApplyBufferSettings();

	void NeigborSearch();
	void Simulate(Merlin::Timestep ts);

	void updateFPS(Merlin::Timestep ts);
private:

	//--- Graphics ---
	Camera_Ptr camera;
	CameraController_Ptr cameraController;

	Scene scene;
	Renderer renderer;

	Shader_Ptr particleShader;
	Shader_Ptr binShader;

	deprecated_ParticleSystem_Ptr particleSystem;
	deprecated_ParticleSystem_Ptr binSystem;

	//--- Solver and sort programs ---
	StagedComputeShader_Ptr solver;
	StagedComputeShader_Ptr prefixSum;

	// --- Buffers ---
	SSBO_Ptr<Bin> binBuffer; //Particle buffer
	SSBO_Ptr<glm::vec2> positionBuffer; // Position buffer
	SSBO_Ptr<glm::vec2> cpyPositionBuffer; // Copy of the position buffer
	SSBO_Ptr<glm::vec2> predictedPositionBuffer; // Predicted position buffer
	SSBO_Ptr<glm::vec2> cpyPredictedPositionBuffer; // Copy of the predicted position buffer
	SSBO_Ptr<glm::vec2> velocityBuffer; // Velocity buffer
	SSBO_Ptr<glm::vec2> cpyVelocityBuffer; // Copy of the velocity buffer
	SSBO_Ptr<float> densityBuffer; // Density buffer
	SSBO_Ptr<float> cpyDensityBuffer; // Copy of the density buffer
	SSBO_Ptr<float> lambdaBuffer; // Lambda buffer
	SSBO_Ptr<float> cpyLambdaBuffer; // Copy of the lambda buffer
	SSBO_Ptr<glm::uvec4> metaBuffer; // Meta buffer containing phase, bin index, id, sortedID
	SSBO_Ptr<glm::uvec4> cpymetaBuffer; // Copy of the meta buffer


	//Simulation
	Settings settings;
	GLuint numParticles = 0;
	GLuint numConstraint = 0;
	GLuint numBoundaryParticles = 0;
	glm::vec3 model_matrix_translation = { 0.0f, 0.0f, 0.0f };

	double nns_time = 0;
	double jacobi_time = 0;
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
	bool mousePressed = false;

};