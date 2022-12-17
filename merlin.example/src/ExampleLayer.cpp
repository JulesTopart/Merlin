#include "ExampleLayer.h"

using namespace Merlin;
using namespace Merlin::Utils;
using namespace Merlin::Renderer;

#include <iostream>
#include <iomanip>

ExampleLayer::ExampleLayer()
	: m_CameraController(45.0f, 16.0f / 9.0f)
{
	m_CameraController.GetCamera().SetPosition(glm::vec3(-2.0f, 0.0f, 1.0f));
}

ExampleLayer::~ExampleLayer()
{

}

void ExampleLayer::OnAttach()
{
	EnableGLDebugging();

	Console::SetLevel(ConsoleLevel::_INFO);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_MULTISAMPLE);

	//m_CameraController.GetCamera().SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));

	m_Shader = std::make_shared<Shader>("default");

	m_Shader->Compile(
		"assets/shaders/axis.vert.glsl",
		"assets/shaders/axis.frag.glsl"
	);

	m_Shader->Use();

	m_VAO = std::make_shared<VAO>();
	m_VAO->Bind();

	/* Cube
	std::vector<Vertex> vertices = {
		Vertex{ glm::vec3(-0.5f, -0.5f, -0.5f)},
		Vertex{ glm::vec3(0.5f, -0.5f, -0.5f)},
		Vertex{ glm::vec3(0.5f,  0.5f, -0.5f)},
		Vertex{ glm::vec3(-0.5f,  0.5f, -0.5f)},
		Vertex{ glm::vec3(-0.5f, -0.5f,  0.5f)},
		Vertex{ glm::vec3(0.5f, -0.5f,  0.5f)},
		Vertex{ glm::vec3(0.5f,  0.5f,  0.5f)},
		Vertex{ glm::vec3(-0.5f,  0.5f,  0.5f)}

		std::vector<GLuint> indices = {
			0, 1, 3, 3, 1, 2,
			1, 5, 2, 2, 5, 6,
			5, 4, 6, 6, 4, 7,
			4, 0, 7, 7, 0, 3,
			3, 2, 7, 7, 2, 6,
			4, 5, 0, 0, 5, 1
		};

	};
	*/
	
	//Axis
	std::vector<Vertex> vertices = {
		Vertex{ glm::vec3(0.2f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)},
		Vertex{ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)},
		Vertex{ glm::vec3(0.0f, 0.2f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
		Vertex{ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
		Vertex{ glm::vec3(0.0f, 0.0f, 0.2f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
		Vertex{ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)}
	};

	m_VBO = std::make_shared<VBO>(vertices);
	m_VAO->AddBuffer(*m_VBO, Vertex::GetLayout());
	m_VAO->Unbind();
	
}

void ExampleLayer::OnDetach()
{
	m_VAO.reset();
}

void ExampleLayer::OnEvent(Event& event)
{
	m_CameraController.OnEvent(event);

	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<MouseButtonPressedEvent>(
		[&](MouseButtonPressedEvent& e)
		{
			if (e.GetMouseButton() == MRL_MOUSE_BUTTON_LEFT) {
				m_SquareColor = m_SquareAlternateColor;
			}else if (e.GetMouseButton() == MRL_MOUSE_BUTTON_RIGHT) {

			}

			return false;
		});
	dispatcher.Dispatch<MouseButtonReleasedEvent>(
		[&](MouseButtonReleasedEvent& e)
		{
			if (e.GetMouseButton() == MRL_MOUSE_BUTTON_LEFT) {
				m_SquareColor = m_SquareBaseColor;
			}else if (e.GetMouseButton() == MRL_MOUSE_BUTTON_RIGHT) {

			}
			return false;
		});
}

void ExampleLayer::OnUpdate(Timestep ts)
{
	
	m_CameraController.OnUpdate(ts);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_Shader->Use();

	m_Shader->SetMatrix4f("view", m_CameraController.GetCamera().GetViewProjectionMatrix());
	//m_Shader->SetUniform4f("lightPos", m_SquareColor);
	//m_Shader->SetUniform4f("color", m_SquareColor);

	m_VAO->Bind();
	glDrawArrays(GL_LINES, 0, 6);
	//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
	m_VAO->Unbind();
	
}

void ExampleLayer::OnImGuiRender()
{

	ImGui::Begin("Camera");

	model_matrix_translation = m_CameraController.GetCamera().GetPosition();
	if (ImGui::DragFloat3("Camera position", &model_matrix_translation.x, -100.0f, 100.0f)) {
		m_CameraController.GetCamera().SetPosition(model_matrix_translation);
	}
	ImGui::End();

	ImGui::Begin("Controles");
	if (ImGui::ColorEdit4("Square Base Color", glm::value_ptr(m_SquareBaseColor)))
		m_SquareColor = m_SquareBaseColor;
	ImGui::ColorEdit4("Square Alternate Color", glm::value_ptr(m_SquareAlternateColor));
	ImGui::End();
}
