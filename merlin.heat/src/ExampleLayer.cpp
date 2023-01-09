#include "ExampleLayer.h"

using namespace Merlin;
using namespace Merlin::Utils;
using namespace Merlin::Renderer;

#include <iostream>
#include <iomanip>

ExampleLayer::ExampleLayer() : cameraController(45.0f, 16.0f / 9.0f){
	cameraController.GetCamera().SetPosition(glm::vec3(-2.0f, 0.0f, 0.0f));
}

ExampleLayer::~ExampleLayer(){
}

void ExampleLayer::OnAttach(){
	EnableGLDebugging();
	Window* wd = &Application::Get().GetWindow();
	_height = wd->GetHeight();
	_width = wd->GetWidth();

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

	physics = CreateShared<ComputeShader>("heat");
	physics->Compile("assets/shaders/particle.heat.glsl");

	//Load models
	axis = ModelLoader::LoadAxis("axis");


	//Particle System settings
	GLsizeiptr gridSize = 16;
	GLsizeiptr partCount = gridSize * gridSize * gridSize;
	float gridWidth = 2.0f;

	//Create particle system
	particleSystem = CreateShared<ParticleSystem>("particle", partCount);
	
	//Define the mesh for instancing (Here a cube)
	Shared<Primitive> cube = Primitive::CreateCube(1.0f);//Primitive::CreateCube(1.0f);
	cube->Translate(glm::vec3(-2, -2, 0));
	particleSystem->SetPrimitive(cube); 

	//Create the buffer
	Shared<SSBO> buffer = CreateShared<SSBO>("ParticleBuffer");
	buffer->SetBindingPoint(1);
	buffer->Allocate<DefaultParticle>(partCount);
	particleSystem->AddStorageBuffer(buffer);

	//Define the compute shaders
	particleSystem->AddComputeShader(physics);

	init->Use();
	init->SetUInt("grid", gridSize);
	init->SetFloat("gridSpacing", gridWidth / float(gridSize));
	particleSystem->Execute(init); //init position using init compute shader

	physics->Use();
	physics->SetFloat("simSpeed", sim_speed);
	physics->SetInt("count", partCount);
	
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
	cameraController.OnEvent(event);
}

void ExampleLayer::OnUpdate(Timestep ts){
	updateFPS(ts);
	cameraController.OnUpdate(ts);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Specify the color of the background
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// Clean the back buffer and depth buffer


	axis->Draw(axisShader, cameraController.GetCamera().GetViewProjectionMatrix());

	//Compute Shader init
	physics->Use();
	physics->SetFloat("simSpeed", sim_speed);
	//update->SetFloat("lightColor", glm::vec3(0.3, 0.3, 0.3));

	if(!paused)particleSystem->Update(ts);
	particleSystem->Draw(particleShader, cameraController.GetCamera().GetViewProjectionMatrix());
}

void ExampleLayer::OnImGuiRender()
{
	ImGui::Begin("Infos");

	model_matrix_translation = cameraController.GetCamera().GetPosition();
	camera_speed = cameraController.GetCameraSpeed();

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
		cameraController.GetCamera().SetPosition(model_matrix_translation);
	}
	if (ImGui::SliderFloat("Camera speed", &camera_speed, 0.0, 100.0f)) {
		cameraController.SetCameraSpeed(camera_speed);
	}

	ImGui::SliderFloat("Simulation speed", &sim_speed, 0.0, 20.0f);
	ImGui::End();
}
