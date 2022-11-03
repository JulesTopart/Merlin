#include "ExampleLayer.h"

using namespace Merlin;
using namespace Merlin::Utils;
using namespace Merlin::Renderer;

ExampleLayer::ExampleLayer()
	: m_CameraController(16.0f / 9.0f)
{

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

	m_Shader = std::make_shared<Shader>("default");
	m_Mesh = std::make_shared<Mesh>("model");

	m_Shader->Compile(
		"assets/shaders/test.vert.glsl",
		"assets/shaders/test.frag.glsl"
	);

	Merlin::Utils::MeshLoader::LoadObject("assets/models/owl.stl");

	m_Shader->Use();


}

void ExampleLayer::OnDetach()
{
	m_Mesh.reset(); //Free mesh
}

void ExampleLayer::OnEvent(Event& event)
{
	m_CameraController.OnEvent(event);

	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<MouseButtonPressedEvent>(
		[&](MouseButtonPressedEvent& e)
		{
			m_SquareColor = m_SquareAlternateColor;
			return false;
		});
	dispatcher.Dispatch<MouseButtonReleasedEvent>(
		[&](MouseButtonReleasedEvent& e)
		{
			m_SquareColor = m_SquareBaseColor;
			return false;
		});
}

void ExampleLayer::OnUpdate(Timestep ts)
{
	
	m_CameraController.OnUpdate(ts);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_Shader->Use();

	//m_Shader->SetMatrix4f("view", m_CameraController.GetCamera().GetViewProjectionMatrix());
	m_Shader->SetUniform4f("color", m_SquareColor);

	m_Mesh->Draw(*m_Shader, m_CameraController.GetCamera());
	
}

void ExampleLayer::OnImGuiRender()
{
	ImGui::Begin("Controls");
	if (ImGui::ColorEdit4("Square Base Color", glm::value_ptr(m_SquareBaseColor)))
		m_SquareColor = m_SquareBaseColor;
	ImGui::ColorEdit4("Square Alternate Color", glm::value_ptr(m_SquareAlternateColor));
	ImGui::End();
}
