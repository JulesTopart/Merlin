#include "ExampleLayer.h"

using namespace Merlin;
using namespace Merlin::Utils;
using namespace Merlin::Renderer;

#include <iostream>
#include <iomanip>

ExampleLayer::ExampleLayer(){
	Window* w = &Application::Get().GetWindow();
	int height = w->GetHeight();
	int width = w->GetWidth();
	camera = CreateShared<Camera>(width, height, Projection::Perspective);
	camera->SetPosition(glm::vec3(-2.0f, 0.0f, 1.0f));
	cameraController = CreateShared<CameraController3D>(camera);
}

ExampleLayer::~ExampleLayer(){

}

void ExampleLayer::OnAttach(){
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
	//model = ModelLoader::LoadCube("cube");
	model = ModelLoader::LoadPlane("plane");
	model->translate(glm::vec3(0, 0, -1));

}

void ExampleLayer::OnDetach(){
	model.reset();
}

void ExampleLayer::OnEvent(Event& event){
	camera->OnEvent(event);
	cameraController->OnEvent(event);
}

void ExampleLayer::OnUpdate(Timestep ts){
	
	cameraController->OnUpdate(ts);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	modelShader->Use();
	modelShader->SetUniform3f("lightPos", glm::vec3(0,0,3));
	modelShader->SetUniform3f("lightColor", glm::vec3(1, 1, 1));
	modelShader->SetUniform3f("viewPos", camera->GetPosition());
	modelShader->SetFloat("shininess", 1.0f);

	model->Draw(modelShader, camera->GetViewProjectionMatrix());
	axis->Draw(axisShader, camera->GetViewProjectionMatrix());

}

void ExampleLayer::OnImGuiRender()
{
	ImGui::Begin("Camera");

	model_matrix_translation = camera->GetPosition();
	if (ImGui::DragFloat3("Camera position", &model_matrix_translation.x, -100.0f, 100.0f)) {
		camera->SetPosition(model_matrix_translation);
	}
	ImGui::End();
}
