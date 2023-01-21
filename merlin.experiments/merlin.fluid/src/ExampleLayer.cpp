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
	//camera->setFOV(60);
	camera->SetPosition(glm::vec3(0.0f, -1.0f, 0.0f));
	cameraController = CreateShared<CameraController3D>(camera);
}

ExampleLayer::~ExampleLayer(){
}

void ExampleLayer::OnAttach(){
	EnableGLDebugging();
	Console::SetLevel(ConsoleLevel::_INFO);
	// Init OpenGL options
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);

	//Shaders for rendering
	axisShader = CreateShared<Shader>("axis");
	axisShader->Compile(
		"assets/shaders/axis.vert.glsl",
		"assets/shaders/axis.frag.glsl"
	);

	particleShader = CreateShared<Shader>("particle");
	particleShader->Compile(
		"assets/shaders/particle.vert.glsl",
		"assets/shaders/particle.frag.glsl"
	);

	//Load Compute shaders for GPU based physics
	init = CreateShared<ComputeShader>("init");
	init->Compile("assets/shaders/particle.init.glsl");

	fluid1 = CreateShared<ComputeShader>("fluid.density");
	fluid1->Compile("assets/shaders/particle.fluid.density.glsl");
	fluid2 = CreateShared<ComputeShader>("fluid.force");
	fluid2->Compile("assets/shaders/particle.fluid.force.glsl");

	//Load models
	axis = Primitive::CreateCoordSystem();
	//axis->Translate(glm::vec3(1));

	//Particle System settings
	GLsizeiptr gridSize = 16;
	GLsizeiptr partCount = gridSize * gridSize * gridSize;
	float gridWidth = 2.0f;

	//Create particle system
	particleSystem = CreateShared<ParticleSystem>("particle", partCount);
	
	//Define the mesh for instancing (Here a cube)
	Shared<Primitive> cube = Primitive::CreateCube(1.0f);//Primitive::CreateCube(1.0f);
	cube->Translate(glm::vec3(0, 0, 0));
	particleSystem->SetPrimitive(cube); 

	//Create the buffer
	Shared<SSBO> buffer = CreateShared<SSBO>("ParticleBuffer");
	buffer->SetBindingPoint(1);
	buffer->Allocate<DefaultParticle>(partCount);
	particleSystem->AddStorageBuffer(buffer);

	//Define the compute shaders
	//particleSystem->AddComputeShader(fluid1);
	particleSystem->AddComputeShader(fluid2);

	init->Use();
	init->SetUInt("grid", gridSize);
	init->SetFloat("gridSpacing", gridWidth / float(gridSize));
	particleSystem->Execute(init); //init position using init compute shader

	//fluid1->Use();
	//fluid1->SetInt("count", partCount);

	fluid2->Use();
	fluid2->SetFloat("simSpeed", sim_speed);
	fluid2->SetInt("count", partCount);
	
	particleShader->Use();
	particleShader->SetFloat("radius", 0.5f * gridWidth / float(gridSize)); //Set particle radius

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

void ExampleLayer::OnEvent(Event& event){
	cameraController->OnEvent(event);
	camera->OnEvent(event);
}

void ExampleLayer::OnUpdate(Timestep ts){
	updateFPS(ts);
	cameraController->OnUpdate(ts);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Specify the color of the background
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// Clean the back buffer and depth buffer

	//Render shader init
	axis->Draw(axisShader, camera->GetViewProjectionMatrix());

	//Compute Shader init


	if(!paused)particleSystem->Update(ts);
	particleSystem->Draw(particleShader, camera->GetViewProjectionMatrix());
}

void ExampleLayer::OnImGuiRender()
{
	ImGui::Begin("Infos");

	model_matrix_translation = camera->GetPosition();
	model_matrix_rotation = camera->GetRotation();
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

	if (ImGui::SmallButton("Reset simulation"))
		particleSystem->Execute(init); //init position using init compute shader

	if (ImGui::DragFloat3("Camera position", &model_matrix_translation.x, -100.0f, 100.0f)) {
		camera->SetPosition(model_matrix_translation);
	}
	if (ImGui::DragFloat3("Camera rotation", &model_matrix_rotation.x, -100.0f, 100.0f)) {
		camera->SetRotation(model_matrix_rotation);
	}
	if (ImGui::SliderFloat("Camera speed", &camera_speed, 0.0, 100.0f)) {
		cameraController->SetCameraSpeed(camera_speed);
	}

	ImGui::SliderFloat("Simulation speed", &sim_speed, 0.0, 20.0f);
	ImGui::End();
}
