#pragma once

#include <Merlin.h>
#include "Solver.h"

using namespace Merlin::Renderer;
using namespace Merlin::Tensor;
using namespace Merlin::Scene;


struct Node {
	alignas(16) glm::vec3 U;		//Position
	alignas(16) glm::vec3 V;		//Velocity

	GLint enable;			//particle is deactivated
	GLuint index;
};

struct IsoVertex {
	glm::vec3 position;//Position
	float scale;
	glm::vec3 normal;//Velocity
	float size;
};


class ExampleLayer : public Merlin::Layer{
public:
	ExampleLayer();
	virtual ~ExampleLayer() {};

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnEvent(Merlin::Event& event) override;
	virtual void OnUpdate(Merlin::Timestep ts) override;
	virtual void OnImGuiRender() override;

	void updateFPS(Merlin::Timestep ts);
	void simulate();

private:

	void CreateMesh();
	void LoadModels();
	void LoadShaders();
	void InitShaders();
	void CreateSolver();

	GLsizei _width, _height;

	//Mesh rendering
	Shared<Shader> axisShader;
	Shared<Shader> voxelShader;
	Shared<Shader> isoSurfaceShader;
	Shared<Primitive> axis;
	Shared<Primitive> voxels;
	Shared<Primitive> isoSurface;

	//Computing
	Shared<ComputeShader> physics;
	Shared<ComputeShader> isosurfaceGen;

	Shared<SSBO> buffer;
	Shared<SSBO> isobuffer;
	Shared<Solver> solver;

	GLsizeiptr nodeCount, sqNodeCount, isoSurfaceResolution;
	float domainWidth;
	//Camera
	Shared<Camera> camera;
	Shared<CameraController> cameraController;

	//ui
	bool drawWiredMesh = false;
	bool drawMesh = true;
	bool paused = true;
	glm::vec3 model_matrix_translation = { 0.0f, 0.0f, 0.0f};

	double t;
	double dt;

	float	sim_speed = 1,
			camera_speed = 1,
			FPS = 0,
			FPS_sample = 0;
};