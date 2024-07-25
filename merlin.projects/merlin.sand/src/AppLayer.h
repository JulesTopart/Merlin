#pragma once
#include "settings.h"
#include "GcodeReader.h"

using namespace Merlin;

#define UNUSED 0
#define SOLID 1
#define FLUID 2
#define FLUID_EMITTER 3
#define GRANULAR 4
#define BOUNDARY 5

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

	GcodeSimulator simulator;

	Scene scene;
	Renderer renderer;
	
	ComputeShader_Ptr isoGen;
	StagedComputeShader_Ptr solver;
	StagedComputeShader_Ptr prefixSum;

	ParticleSystem_Ptr ps;
	ParticleSystem_Ptr bs;

	Shader_Ptr particleShader;
	Shader_Ptr binShader;

	// --- Simulation--- 
	Settings settings;
	
	glm::vec3 laser_position = { 0.0f, 0.0f, 0.0f };
	glm::vec3 model_matrix_translation = { 0.0f, 0.0f, 0.0f };

	Mesh_Ptr floor;
	Mesh_Ptr powder;

	IsoSurface_Ptr isosurface;
	Texture3D_Ptr volume;
	Texture2D_Ptr texture_debug;

	SpotLight_Ptr laser;
	Texture2D_Ptr laser_map;//depth

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
	bool use_emitter = true;
	bool use_isosurface = false;

};