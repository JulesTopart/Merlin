#include "ExampleLayer.h"

using namespace Merlin;
using namespace Merlin::Utils;
using namespace Merlin::Renderer;

#include <iostream>
#include <iomanip>

ExampleLayer::ExampleLayer()
	: cameraController(45.0f, 16.0f / 9.0f)
{
	cameraController.GetCamera().SetPosition(glm::vec3(-2.0f, 0.0f, 1.0f));
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

	axisShader = std::make_shared<Shader>("axis");

	axisShader->Compile(
		"assets/shaders/axis.vert.glsl",
		"assets/shaders/axis.frag.glsl"
	);

	modelShader = std::make_shared<Shader>("model");

	modelShader->Compile(
		"assets/shaders/model.vert.glsl",
		"assets/shaders/model.frag.glsl"
	);
	
	axis = ModelLoader::LoadAxis("axis");
	model = ModelLoader::LoadCube("cube");

	axis->LinkShader();
}

void ExampleLayer::OnDetach()
{
	model.reset();
}

void ExampleLayer::OnEvent(Event& event)
{
	cameraController.OnEvent(event);

	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<MouseButtonPressedEvent>(
		[&](MouseButtonPressedEvent& e)
		{
			if (e.GetMouseButton() == MRL_MOUSE_BUTTON_LEFT) {
				
			}else if (e.GetMouseButton() == MRL_MOUSE_BUTTON_RIGHT) {

			}

			return false;
		});
	dispatcher.Dispatch<MouseButtonReleasedEvent>(
		[&](MouseButtonReleasedEvent& e)
		{
			if (e.GetMouseButton() == MRL_MOUSE_BUTTON_LEFT) {
				
			}else if (e.GetMouseButton() == MRL_MOUSE_BUTTON_RIGHT) {

			}
			return false;
		});
}

void ExampleLayer::OnUpdate(Timestep ts)
{
	
	cameraController.OnUpdate(ts);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	

	shader->SetMatrix4f("view", m_CameraController.GetCamera().GetViewProjectionMatrix());
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
