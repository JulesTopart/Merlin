#pragma once

#include <Merlin.h>

using namespace Merlin::Renderer;

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
	
	std::shared_ptr<Shader> modelShader;
	std::shared_ptr<Shader> axisShader;
	std::shared_ptr<Mesh> axis;
	std::shared_ptr<Mesh> model;

	std::shared_ptr<FBO> msaa_fbo;
	std::shared_ptr<FBO> fbo;
	std::shared_ptr<ScreenQuadRenderer> screen;

	GLuint emptyVAO = 0;

	Merlin::Scene::CameraController cameraController;

	glm::vec3 model_matrix_translation = { 0.0f, 0.0f, 0.0f};
};