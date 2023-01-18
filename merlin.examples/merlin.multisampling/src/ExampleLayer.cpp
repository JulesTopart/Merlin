#include "ExampleLayer.h"

using namespace Merlin;
using namespace Merlin::Utils;
using namespace Merlin::Renderer;

#include <iostream>
#include <iomanip>

ExampleLayer::ExampleLayer() {
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
	Window* wd = &Application::Get().GetWindow();
	_height = wd->GetHeight();
	_width = wd->GetWidth();

	Console::SetLevel(ConsoleLevel::_INFO);

	// Init OpenGL stuff
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CW);

	screen = std::make_shared<ScreenQuadRenderer>();

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


	// Create MSAA Frame Buffer Object
	msaa_fbo = std::make_shared<FBO>(_width, _height );
	msaa_fbo->Bind();

	//Create Texture  attachement
	msaa_fbo->AddColorAttachment(msaa_fbo->CreateTextureAttachment(GL_RGBA, 4));
	// Create Render Buffer Object
	msaa_fbo->AddDepthStencilAttachment(msaa_fbo->CreateRenderBufferAttachment(GL_DEPTH24_STENCIL8, 4));


	// Create non MSAA Frame Buffer Object
	fbo = std::make_shared<FBO>(_width, _height);
	fbo->Bind();

	//Create Texture  attachement
	fbo->AddColorAttachment(fbo->CreateTextureAttachment(GL_RGBA, 0));


}

void ExampleLayer::OnDetach(){}

void ExampleLayer::OnEvent(Event& event) {
	camera->OnEvent(event);
	cameraController->OnEvent(event);
}

void ExampleLayer::OnUpdate(Timestep ts) {

	cameraController->OnUpdate(ts);

	// Bind the custom framebuffer
	msaa_fbo->Bind();
	// Specify the color of the background
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	// Clean the back buffer and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Enable depth testing since it's disabled when drawing the framebuffer rectangle
	glEnable(GL_DEPTH_TEST);

	modelShader->Use();
	modelShader->SetUniform3f("lightPos", glm::vec3(0,0,3));
	modelShader->SetUniform3f("lightColor", glm::vec3(1, 1, 1));
	modelShader->SetUniform3f("viewPos", camera->GetPosition());
	modelShader->SetFloat("shininess", 1.0f);

	model->Draw(modelShader, camera->GetViewProjectionMatrix());
	axis->Draw(axisShader, camera->GetViewProjectionMatrix());

	msaa_fbo->Bind(GL_READ_FRAMEBUFFER);
	fbo->Bind(GL_DRAW_FRAMEBUFFER);

	// Copy the multisampled framebuffer to the non-multisampled framebuffer, applying multisample resolve filters as needed
	glBlitFramebuffer(0, 0, _width, _height, 0, 0, _width, _height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	// Bind the default framebuffer
	msaa_fbo->Unbind();

	glDisable(GL_DEPTH_TEST); // prevents framebuffer rectangle from being discarded
	screen->Draw(msaa_fbo->GetColorAttachment(0));

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
