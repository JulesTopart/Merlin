#pragma once
#include "settings.h"

using namespace Merlin;

#define UNUSED 0
#define SOLID 1
#define FLUIDA 2
#define FLUIDB 3
#define FLUID_EMITTER 4
#define GRANULAR 5
#define BOUNDARY 6

class AppLayer : public Merlin::Layer3D {
public:
	AppLayer(){}

	void onAttach() override;
	void onDetach() override;
	void onEvent(Merlin::Event& event) override;
	void onUpdate(Merlin::Timestep ts) override;
	void onImGuiRender() override;

	void InitGraphics();
	void InitPhysics();
	void ResetSimulation();

	void SyncUniforms();

	void NeigborSearch();
	void Simulate(Merlin::Timestep ts);

	void SpawnParticle();

private:

	// --- Graphics ---

	Scene scene;
	Renderer renderer;
	
	StagedComputeShader_Ptr solver;
	StagedComputeShader_Ptr prefixSum;

	ParticleSystem_Ptr ps;
	ParticleSystem_Ptr bs;

	Shader_Ptr particleShader;
	Shader_Ptr binShader;

	// --- Simulation--- 
	Settings settings;

	Mesh_Ptr floor;
	Mesh_Ptr nozzle;
	Mesh_Ptr intermixer;
	Mesh_Ptr static_emitterA;
	Mesh_Ptr static_emitterB;
	Mesh_Ptr capA;
	Mesh_Ptr capB;

	
	glm::vec3 model_matrix_translation = { 0.0f, 0.0f, 0.0f };

	double nns_time = 0;
	double jacobi_time = 0;
	double solver_substep_time = 0;
	double solver_total_time = 0;
	double render_time = 0; double render_start_time = 0;
	double total_time = 0; double total_start_time = 0;

	float lastSpawTime = 0;
	
	float elapsedTime = 0;
	bool paused = true;
	bool integrate = true;
	float sim_speed = 1;
	float camera_speed = 1;
	bool mousePressed = false;

	bool use_real_time = false;
	bool use_emitter = false;

};