#pragma once
#include "settings.h"

using namespace Merlin::Graphics;

#define UNUSED 0
#define SOLID 1
#define FLUID 2
#define GAS 3
#define GRANULAR 4
#define BOUNDARY 5

class AppLayer : public Merlin::Layer {
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

	void SyncUniforms();
	void ApplyBufferSettings();

	void NeigborSearch();
	void Simulate(Merlin::Timestep ts);

	void updateFPS(Merlin::Timestep ts);
private:

	Camera_Ptr camera;
	CameraController_Ptr cameraController;

	Scene scene;
	Renderer renderer;

	//ScreenQuadRenderer qrenderer;

	//Simulation
	StagedComputeShader_Ptr solver;
	StagedComputeShader_Ptr prefixSum;

	SSBO_Ptr<Bin> binBuffer; //Particle buffer
	SSBO_Ptr<glm::vec4> positionBuffer; // Position buffer
	SSBO_Ptr<glm::vec4> cpyPositionBuffer; // Copy of the position buffer
	SSBO_Ptr<glm::vec4> predictedPositionBuffer; // Predicted position buffer
	SSBO_Ptr<glm::vec4> cpyPredictedPositionBuffer; // Copy of the predicted position buffer
	SSBO_Ptr<glm::vec4> velocityBuffer; // Velocity buffer
	SSBO_Ptr<glm::vec4> cpyVelocityBuffer; // Copy of the velocity buffer
	SSBO_Ptr<float> densityBuffer; // Density buffer
	SSBO_Ptr<float> cpyDensityBuffer; // Copy of the density buffer
	SSBO_Ptr<float> lambdaBuffer; // Lambda buffer
	SSBO_Ptr<float> cpyLambdaBuffer; // Copy of the lambda buffer
	SSBO_Ptr<glm::uvec4> metaBuffer; // Meta buffer containing phase, bin index, id, sortedID
	SSBO_Ptr<glm::uvec4> cpymetaBuffer; // Copy of the meta buffer

	SSBO_Ptr<glm::vec4> buffer_vertices; // Copy of the meta buffer
	SSBO_Ptr<glm::vec4> buffer_normals; // Copy of the meta buffer
	SSBO_Ptr<int> buffer_triangle_table; // Copy of the meta buffer
	SSBO_Ptr<int> buffer_configuration_table; // Copy of the meta buffer

	uint32_t vao_draw;
	size_t max_number_of_vertices;


	Shader_Ptr modelShader;
	Shader_Ptr particleShader;
	Shader_Ptr binShader;

	deprecated_ParticleSystem_Ptr particleSystem;
	deprecated_ParticleSystem_Ptr binSystem;
	Settings settings;

	//Simulation
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