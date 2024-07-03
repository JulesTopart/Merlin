#pragma once
#include "settings.h"

using namespace Merlin;

#define UNUSED 0
#define SOLID 1
#define FLUID 2
#define GAS 3
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

private:

	// --- Graphics ---

	Scene scene;
	Renderer renderer;
	
	StagedComputeShader_Ptr solver;
	ParticleSystem_Ptr voxels;

	Texture3D_Ptr voxels_temp_in;
	Texture3D_Ptr voxels_temp_out;

	Shader_Ptr binShader;

	// --- Simulation--- 
	Settings settings;
	GLuint numVoxels = 0;

	Mesh_Ptr bunny;
	
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
	bool mousePressed = false;

};