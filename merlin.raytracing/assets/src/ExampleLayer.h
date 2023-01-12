#pragma once

#include <Merlin.h>

using namespace Merlin::Renderer;
using namespace Merlin::Scene;
using namespace Merlin::Tensor;

struct Ray {
	glm::vec3 origin; //Origin
	glm::vec3 direction; //Direction
	int hitID;
	int bounce;
};


class ExampleLayer : public Merlin::Layer
{
public:
	ExampleLayer();
	virtual ~ExampleLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnEvent(Merlin::Event& event) override;
	virtual void OnUpdate(Merlin::Timestep ts) override;
	virtual void OnImGuiRender() override;

	void updateFPS(Merlin::Timestep ts);

private:
	GLsizei _width = 1080, _height = 720;
	
	//Rendering
	Shared<Shader> axisShader;
	Shared<Shader> modelShader;

	Shared<Primitive> axis;
	Shared<Primitive> sphere;
	Shared<Primitive> model; //Geometry

	//Computing
	Shared<ParticleSystem> rays;
	Shared<SSBO> rayBuffer;

	Shared<ComputeShader> init;
	Shared<ComputeShader> raytracing;

	//Camera
	CameraController cameraController;

	//Ui
	glm::vec3 camera_translation = { 0.0f, 0.0f, 0.0f };
	float camera_speed = 1;
	float FPS = 0;
	float FPS_sample = 0;
};