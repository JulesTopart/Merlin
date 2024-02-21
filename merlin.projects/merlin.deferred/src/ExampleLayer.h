#pragma once

#include <Merlin.h>
using namespace Merlin::Graphics;

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
private:
	GLsizei _width = 1080, _height = 720;
	Camera_Ptr camera;
	CameraController_Ptr cameraController;

	Scene scene;
	Renderer renderer;
	FBO_Ptr fbo;
	ScreenQuadRenderer_Ptr screen;

	Model_Ptr  light;
	SkyBox_Ptr sky;
	Shader_Ptr modelShader;

	int bufferID = 0;
	glm::vec3 model_matrix_translation = { 0.8f, 0.2f, 0.3f};
};