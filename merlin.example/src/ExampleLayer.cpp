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

	Console::SetLevel(ConsoleLevel::_TRACE);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_MULTISAMPLE);

	m_Shader = std::make_shared<Shader>("default");

	m_Shader->Compile(
		"assets/shaders/test.vert.glsl",
		"assets/shaders/test.frag.glsl"
	);

	m_Shader->Use();

	m_VAO = std::make_shared<VAO>();
	m_VAO->Bind();

	std::vector<Vertex> vertices = {
		Vertex{ glm::vec3(-0.5f, -0.5f, 0.0f)},
		Vertex{ glm::vec3( 0.5f, -0.5f, 0.0f)},
		Vertex{ glm::vec3( 0.5f,  0.5f, 0.0f)},
		Vertex{ glm::vec3(-0.5f,  0.5f, 0.0f)}
	};

	std::vector<GLuint> indices = { 0, 1, 2, 2, 3, 0 };

	m_VBO = std::make_shared<VBO>(vertices);
	m_EBO = std::make_shared<EBO>(indices);
	

	VertexBufferLayout layout;
	layout.Push<float>(3); //Vertex pos
	layout.Push<float>(3); //Vertex normal
	layout.Push<float>(3); //Vertex color
	layout.Push<float>(2); //Texture coordinates
	m_VAO->AddBuffer(*m_VBO, layout);

	m_VAO->Unbind();
	m_EBO->Unbind();
	


}

void ExampleLayer::OnDetach()
{
	m_VAO.reset();
	m_EBO.reset();
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

	m_Shader->SetMatrix4f("view", m_CameraController.GetCamera().GetViewProjectionMatrix());
	m_Shader->SetUniform4f("color", m_SquareColor);

	m_VAO->Bind();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	m_VAO->Unbind();
	
}

void ExampleLayer::OnImGuiRender()
{
	ImGui::Begin("Controls");
	if (ImGui::ColorEdit4("Square Base Color", glm::value_ptr(m_SquareBaseColor)))
		m_SquareColor = m_SquareBaseColor;
	ImGui::ColorEdit4("Square Alternate Color", glm::value_ptr(m_SquareAlternateColor));
	ImGui::End();
}
