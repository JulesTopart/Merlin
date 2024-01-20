#pragma once
#include <Merlin.h>
#include "settings.h"

using namespace Merlin::Graphics;


class TemplateLayer : public Merlin::Layer
{
public:
	TemplateLayer();
	virtual ~TemplateLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnEvent(Merlin::Event& event) override;
	virtual void OnUpdate(Merlin::Timestep ts) override;
	virtual void OnImGuiRender() override;
private:

	Shared<Camera> camera;

	Scene scene;
	Renderer renderer;

	Shared<Model>  light;




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

	glm::vec3 model_matrix_translation = { 0.8f, 0.2f, 0.3f};
};