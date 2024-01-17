#pragma once

#include <Merlin.h>
#include "Solver.h"

using namespace Merlin::Renderer;
using namespace Merlin::Tensor;
using namespace Merlin::Scene;

/*
struct Node {
	GLdouble temperature;
	alignas(16) glm::vec3 position;
};
*/


struct Node {
	alignas(16) glm::vec3 U;		//Position
	alignas(16) glm::vec3 V;		//Velocity

	float h;	//Smoothing radius
	float d;	//Density
	float p;	//Pressure
	float T;	//Temperature

	alignas(16) glm::vec4 S;		//stress [Sxx, Sxy, Syy, Szz]
	alignas(16) glm::vec4 R;		//artificial Stress [Rxx, Rxy, Ryy, -]

	alignas(16) glm::vec4 S_d;	//dS(u)/du Stress spatial derivative
	alignas(16) glm::vec4 V_d;	//dV(u)/du ? spatial derivative

	//plastic strain and strain rate
	float eps_pl;
	float eps_pl_dot;

	//Time derivatives
	alignas(16) glm::vec3 U_t;	//dU(t)/dt
	alignas(16) glm::vec3 V_t;	//dV(t)/dt

	float d_t;	//dd(t)/dt
	alignas(16) glm::vec4  S_t;	//dS(t)/dt
	float T_t;	//dT(t)/dt

	//Boundary condition
	float fix;				//particle is fixed in space
	float enable;			//particle is deactivated
};

struct Color {
	alignas(16) glm::vec3 color;
	float value;
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

private:

	void CreateMesh();
	void LoadModels();
	void LoadShaders();
	void SetColorGradient();
	void InitShaders();
	void CreateSolver();


	void saveData();

	GLsizei _width, _height;
	
	//Mesh rendering
	Shared<Shader> axisShader;
	Shared<Shader> legendShader;
	Shared<Shader> meshShader;
	Shared<Primitive> axis;
	Shared<Primitive> legend;

	Shared<Primitive> mesh;

	Shared<SSBO> heatMap;
	int colorCount;

	//Computing
	Shared<ComputeShader> physics;
	Shared<ComputeShader> init;

	Shared<SSBO> buffer;
	Shared<Solver> solver;
	int nodeCount, sqNodeCount;
	float domainWidth;

	//Camera
	Shared<Camera> camera;
	Shared<CameraController> cameraController;

	//Time
	double t;
	double dt;

	//ui
	bool drawWiredMesh = false;
	bool drawMesh = true;
	bool paused = true;
	glm::vec3 model_matrix_translation = { 0.0f, 0.0f, 0.0f};

	float	sim_speed = 1,
			camera_speed = 1,
			FPS = 0,
			FPS_sample = 0;
};