#include "ExampleLayer.h"

using namespace Merlin;
using namespace Merlin::Utils;
using namespace Merlin::Renderer;

#include <iostream>
#include <iomanip>

ExampleLayer::ExampleLayer() : cameraController(45.0f, 16.0f / 9.0f){
	cameraController.GetCamera().SetPosition(glm::vec3(-2.0f, 0.0f, 1.0f));
}

ExampleLayer::~ExampleLayer(){

}

void ExampleLayer::OnAttach(){
	EnableGLDebugging();
	Window* wd = &Application::Get().GetWindow();
	_height = wd->GetHeight();
	_width = wd->GetWidth();

	Console::SetLevel(ConsoleLevel::_INFO);

	// Init OpenGL stuff
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_MULTISAMPLE);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);
	//glFrontFace(GL_CW);

	screen = std::make_shared<ScreenQuadRenderer>();

	spframe = FrameBuffer::Create(_height, _width); //MSAA FrameBuffer
	spframe->AddColorAttachment(spframe->CreateTextureAttachment(GL_RGBA,4));

	frame = FrameBuffer::Create(_height, _width); //PostProcessing FrameBuffer
	frame->AddColorAttachment(frame->CreateTextureAttachment(GL_RGBA, 0));

	//Textures
	//tex = std::make_shared<Texture>(0);
	//tex->LoadFromFile("assets/textures/wall.jpg", Texture::Type::DIFFUSE, GL_RGB);

	//Shaders
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
	
	//Load models
	axis = ModelLoader::LoadAxis("axis");
	model = ModelLoader::LoadPlane("plane");
	model->translate(glm::vec3(0, 0, -1));
}

void ExampleLayer::OnDetach(){}

void ExampleLayer::OnEvent(Event& event){
	cameraController.OnEvent(event);
}

void ExampleLayer::OnUpdate(Timestep ts){
	
	glEnable(GL_DEPTH_TEST);
	spframe->Bind();

	cameraController.OnUpdate(ts);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	modelShader->Use();
	modelShader->SetUniform3f("lightPos", glm::vec3(0,0,3));
	modelShader->SetUniform3f("lightColor", glm::vec3(1, 1, 1));
	modelShader->SetUniform3f("viewPos", cameraController.GetCamera().GetPosition());
	modelShader->SetFloat("shininess", 1.0f);

	model->Draw(*modelShader, cameraController.GetCamera().GetViewProjectionMatrix());
	axis->Draw(*axisShader, cameraController.GetCamera().GetViewProjectionMatrix());


	spframe->Bind(GL_READ_FRAMEBUFFER);
	frame->Bind(GL_DRAW_FRAMEBUFFER);
	// Copy the multisampled framebuffer to the non-multisampled framebuffer, applying multisample resolve filters as needed
	glBlitFramebuffer(0, 0, _width, _height, 0, 0, _width, _height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	screen->Draw(frame->GetColorAttachment(0));
	//screen->Draw(tex);

}

void ExampleLayer::OnImGuiRender()
{
	ImGui::Begin("Camera");

	model_matrix_translation = cameraController.GetCamera().GetPosition();
	if (ImGui::DragFloat3("Camera position", &model_matrix_translation.x, -100.0f, 100.0f)) {
		cameraController.GetCamera().SetPosition(model_matrix_translation);
	}
	ImGui::End();
}
