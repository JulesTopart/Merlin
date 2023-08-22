#include "ExampleLayer.h"

using namespace Merlin;
using namespace Merlin::Utils;
using namespace Merlin::Graphics;

#include <iostream>
#include <iomanip>

GLsizei gridX;
GLsizei gridY;
GLsizei gsIt = 50; //Gauss Seidel iteration count
GLsizeiptr gridSize;

ExampleLayer::ExampleLayer() {
	Window* w = &Application::Get().GetWindow();
	gridX = _width = w->GetHeight();
	gridY = _height = w->GetWidth();
	gridSize = gridX * gridY;

	camera = CreateShared<Camera>(_width, _height, Projection::Orthographic);
	camera->setNearPlane(0.001f);
	camera->setFarPlane(3000.0f);
	camera->setFOV(45.0f); //Use 90.0f as we are using cubemaps
	camera->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	camera->SetRotation(glm::vec3(0, 0, 0));
	cameraController = CreateShared<CameraController2D>(camera);
}

ExampleLayer::~ExampleLayer(){}

void ExampleLayer::InitGraphics() {

	renderer = CreateShared<ScreenQuadRenderer>();

	velocity = CreateShared<Texture>();
	velocity->Bind();
	velocity->Allocate(gridX, gridY, GL_RG, GL_RG32F);
	velocity->SetUnit(0);

	phase = CreateShared<Texture>();
	phase->Bind();
	phase->Allocate(gridX, gridY, GL_RED, GL_R32F);
	phase->SetUnit(1);

	pressure = CreateShared<Texture>();
	pressure->Bind();
	pressure->Allocate(gridX, gridY, GL_RED, GL_R32F);
	pressure->SetUnit(2);

}

void ExampleLayer::BindImages() {
	velocity->BindImage();
	phase->BindImage();
	pressure->BindImage();
}

void ExampleLayer::SyncTextureUnits(ShaderBase& sh) {
	velocity->SyncTextureUnit(sh, "velocity");
	phase->SyncTextureUnit(sh, "phase");
	pressure->SyncTextureUnit(sh, "pressure");
}

void ExampleLayer::InitPhysics() {

	//Compute Shaders
	init = std::make_shared<ComputeShader>("init");
	init->Compile("assets/kernels/fvm.init.glsl");
	project = std::make_shared<ComputeShader>("physics");
	project->Compile("assets/kernels/fvm.project.glsl");
	advect = std::make_shared<ComputeShader>("advect");
	advect->Compile("assets/kernels/fvm.advect.glsl");
	pre = std::make_shared<ComputeShader>("pre");
	pre->Compile("assets/kernels/fvm.pre.glsl");

	init->Use();
	SyncTextureUnits(*init);
	init->SetFloat("width", gridX);
	init->SetFloat("height", gridY);

	project->Use();
	SyncTextureUnits(*project);
	project->SetFloat("dt", 0.0016);
	project->SetFloat("it", gsIt);
	project->SetFloat("speed", sim_speed);
	project->SetFloat("width", gridX);
	project->SetFloat("height", gridY);

	advect->Use();
	SyncTextureUnits(*advect);
	advect->SetFloat("dt", 0.0016);
	advect->SetFloat("speed", sim_speed);
	advect->SetFloat("width", gridX);
	advect->SetFloat("height", gridY);

	pre->Use();
	SyncTextureUnits(*pre);

	init->Use();
	BindImages();
	init->Dispatch(gridX, gridY);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

}

void ExampleLayer::Simulate() {

	BindImages();
	pre->Use();
	pre->Dispatch(gridX, gridY);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	project->Use();
	project->SetFloat("speed", sim_speed);

	for (int i = 0; i < gsIt; i++) {
		project->Dispatch(gridX, gridY);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
	}

	advect->Use();
	advect->SetFloat("speed", sim_speed);
	//advect->Dispatch(gridX, gridY);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void ExampleLayer::SetColorGradient() {

	std::vector<Color> colors;
	colors.push_back({ glm::vec3(0, 0, 1), 0.0f });     // Blue.
	colors.push_back({ glm::vec3(0, 1, 1), 0.25f });     // Cyan.
	colors.push_back({ glm::vec3(0, 1, 0), 0.5f });     // Green.
	colors.push_back({ glm::vec3(1, 1, 0), 0.75f });     // Yellow.
	colors.push_back({ glm::vec3(1, 0, 0), 1.0f });     // Red.
	colorCount = colors.size();

	heatMap = CreateShared<SSBO>("ColorMapBuffer");
	heatMap->SetBindingPoint(0);
	heatMap->Allocate<Color>(colors);
}


void ExampleLayer::OnAttach(){
	EnableGLDebugging();
	Window* wd = &Application::Get().GetWindow();
	_height = wd->GetHeight();
	_width = wd->GetWidth();
	Console::SetLevel(ConsoleLevel::_INFO);

	InitGraphics();
	InitPhysics();
	SetColorGradient();
}

void ExampleLayer::OnDetach(){}

void ExampleLayer::updateFPS(Timestep ts) {
	if (FPS_sample == 0) {
		FPS = ts;
	}else {
		FPS += ts;
	}
	FPS_sample++;
}

void ExampleLayer::OnEvent(Event& event) {
	camera->OnEvent(event);
	cameraController->OnEvent(event);
}

void ExampleLayer::OnUpdate(Timestep ts) {
	renderer->Clear();
	cameraController->OnUpdate(ts);
	updateFPS(ts);

	if (!paused) {

		Simulate();
	}


	heatMap->Bind();
	renderer->GetShader()->Use();
	renderer->GetShader()->SetUInt("colorCount", 5);
	glDisable(GL_DEPTH_TEST); // prevents framebuffer rectangle from being discarded
	renderer->Render(velocity);


}

void ExampleLayer::OnImGuiRender()
{
	ImGui::Begin("Infos");

	model_matrix_translation = camera->GetPosition();
	camera_speed = cameraController->GetCameraSpeed();

	if (FPS_sample > 0) {
		ImGui::LabelText("FPS", std::to_string(1.0f / (FPS / FPS_sample)).c_str());
		if (FPS_sample > 50) FPS_sample = 0;
	}

	if (paused) {
		if (ImGui::ArrowButton("Run simulation", 1))
			paused = !paused;
	}
	else {
		if (ImGui::SmallButton("Pause simulation"))
			paused = !paused;
	}

	if (ImGui::SmallButton("Reset simulation")) {
		init->Use();
		BindImages();
		init->Dispatch(_width, _height);
	}
	if (ImGui::DragFloat3("Camera position", &model_matrix_translation.x, -100.0f, 100.0f)) {
		camera->SetPosition(model_matrix_translation);
	}
	if (ImGui::SliderFloat("Camera speed", &camera_speed, 0.0, 50.0f)) {
		cameraController->SetCameraSpeed(camera_speed);
	}
	if (ImGui::SliderFloat("Simulation speed", &sim_speed, 0.0, 5.0f)) {
	}


	ImGui::End();
}


