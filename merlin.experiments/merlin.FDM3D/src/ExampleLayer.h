#pragma once

#include <Merlin.h>
#include "Solver.h"

using namespace Merlin::Graphics;
using namespace Merlin::Tensor;
using namespace Merlin::Memory;

/*
struct Node {
	GLdouble temperature;
	alignas(16) glm::vec3 position;
};
*/

struct Bin {
	int start;
	int end;
};

struct Node {
	alignas(16) glm::vec3 U;		//Position
	alignas(16) glm::vec3 V;		//Velocity

	GLfloat h;	//Smoothing radius
	GLfloat d;	//Density
	GLfloat p;	//Pressure
	GLfloat T;	//Temperature

	alignas(16) glm::vec4 S;		//stress [Sxx, Sxy, Syy, Szz]
	alignas(16) glm::vec4 R;		//artificial Stress [Rxx, Rxy, Ryy, -]

	alignas(16) glm::vec4 S_d;	//dS(u)/du Stress spatial derivative
	alignas(16) glm::vec4 V_d;	//dV(u)/du ? spatial derivative

	//plastic strain and strain rate
	GLfloat eps_pl;
	GLfloat eps_pl_dot;

	//Time derivatives
	alignas(16) glm::vec3 U_t;	//dU(t)/dt
	alignas(16) glm::vec3 V_t;	//dV(t)/dt

	GLfloat d_t;	//dd(t)/dt
	alignas(16) glm::vec4  S_t;	//dS(t)/dt
	GLfloat T_t;	//dT(t)/dt

	//Boundary condition
	GLint fix;				//particle is fixed in space
	GLint enable;			//particle is deactivated
	GLuint index;
};

struct Color {
	alignas(16) glm::vec3 color;
	GLfloat value;
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
	void SetColorGradient();
	void InitShaders();
	void CreateSolver();

	GLsizei _width, _height;

	//Moving heat source
	glm::vec3 u;
	glm::vec3 v;
	float speed;
	int line = 0;

	//Mesh rendering
	Shared<Shader> axisShader;
	Shared<Shader> modelShader;
	Shared<Shader> voxelShader;

	//Computing
	Shared<ComputeShader> physics;
	Shared<ComputeShader> init;

	SSBO buffer;
	SSBO binBuffer;
	Shared<Solver> solver;
	GLsizeiptr nodeCount, sqNodeCount;
	float domainWidth;
	int hashResolution;
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