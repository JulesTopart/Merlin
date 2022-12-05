#pragma once

#include <Merlin.h>
#include <MerlinRenderer.h>
#include <MerlinUtils.h>

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
	std::shared_ptr<Merlin::Renderer::Shader> m_Shader;
	std::shared_ptr<Merlin::Renderer::VertexArray> m_VAO;
	std::shared_ptr<Merlin::Renderer::VertexBuffer> m_VBO;

	Merlin::Utils::CameraController m_CameraController;

	glm::vec4 m_SquareBaseColor = { 0.8f, 0.2f, 0.3f, 1.0f };
	glm::vec4 m_SquareAlternateColor = { 0.2f, 0.3f, 0.8f, 1.0f };
	glm::vec4 m_SquareColor = m_SquareBaseColor;
	glm::vec3 model_matrix_translation = { 0.8f, 0.2f, 0.3f};
};