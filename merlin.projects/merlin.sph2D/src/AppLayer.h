#pragma once
#include <Merlin.h>
#include "settings.h"

using namespace Merlin::Graphics;

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

	void OnAttach() override;
	void OnDetach() override;
	void OnEvent(Merlin::Event& event) override;
	void OnUpdate(Merlin::Timestep ts) override;
	void OnImGuiRender() override;

	void InitGraphics();
	void InitPhysics();
	void ResetSimulation();
	void UpdateBufferSettings();

	void NeigborSearch();
	void Simulate(Merlin::Timestep ts);

	void updateFPS(Merlin::Timestep ts);
private:

	Camera camera;

	Scene scene;
	Renderer renderer;

	//Simulation
	StagedComputeShader solver;
	StagedComputeShader prefixSum;

	SSBO<Bin> binBuffer; //Particle buffer
	SSBO<Particle> particleBuffer; //Particle buffer

	Shader particleShader;
	Shader binShader;

	deprecated_ParticleSystem particleSystem;
	deprecated_ParticleSystem binSystem;
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