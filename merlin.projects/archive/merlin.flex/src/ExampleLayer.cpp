#include "ExampleLayer.h"

using namespace Merlin;
using namespace Merlin::Utils;
using namespace Merlin::Graphics;

#include <iostream>
#include <iomanip>

ExampleLayer::ExampleLayer() {
	Window* w = &Application::Get().GetWindow();
	int height = w->GetHeight();
	int width = w->GetWidth();
	camera = CreateShared<Camera>(width, height, Projection::Perspective);
	camera->setNearPlane(0.1f);
	camera->setFarPlane(3000.0f);
	camera->setFOV(45.0f); //Use 90.0f as we are using cubemaps
	camera->SetPosition(glm::vec3(0.0f, -240.0f, 200.0));
	camera->SetRotation(glm::vec3(0, 40, -270));
	cameraController = CreateShared<CameraController3D>(camera);
}

ExampleLayer::~ExampleLayer(){}



void ExampleLayer::InitGraphics() {
	// Init OpenGL stuff
	renderer.Initialize();
	renderer.SetBackgroundColor(0.203, 0.203, 0.203, 1.0);

	//Shaders
	modelShader = Shader::Create("model", "assets/shaders/model.vert", "assets/shaders/model.frag");
	modelShader->SetVec3("lightPos", glm::vec3(0, 0, 20));
	modelShader->SetVec4("lightColor", glm::vec4(100));

	particleShader = Shader::Create("particle", "assets/shaders/particle.vert", "assets/shaders/particle.frag");
	particleShader->noTexture();
	particleShader->noMaterial();
	particleShader->SetUInt("colorCount", 5);
	particleShader->SetVec4("lightColor", glm::vec4(1));
	particleShader->SetVec3("lightPos", glm::vec3(0, 0, 20));

	renderer.AddShader(modelShader);
	renderer.AddShader(particleShader);

	renderer.EnableTransparency();
	renderer.EnableSampleShading();


	//SkyBox
	std::vector<std::string> skyBoxPath = {
		"./assets/textures/skybox/right.jpg",
		"./assets/textures/skybox/left.jpg",
		"./assets/textures/skybox/bottom.jpg",
		"./assets/textures/skybox/top.jpg",
		"./assets/textures/skybox/front.jpg",
		"./assets/textures/skybox/back.jpg"
	};

	Shared<Shader> skyShader = Shader::Create("skybox", "assets/shaders/skybox.vert", "assets/shaders/skybox.frag");
	Shared<SkyBox> sky = SkyBox::Create("Sky", skyBoxPath);
	sky->SetShader(skyShader);
	scene.Add(sky);

	//Floor
	//Shared<Model> floor = Model::Create("floor", Primitives::CreateCube(10.0, 10.0, 0.1));
	//floor->Translate(glm::vec3(-0.5, -0.4, -(0.75 + 1.0 / 32.0)));

	Shared<Model> floor = ModelLoader::LoadModel("./assets/models/bed.stl");
	floor->Translate(glm::vec3(0,0,0));

	Shared<Material> floorMat = CreateShared<Material>("floorMat");
	floorMat->SetAmbient(glm::vec3(0.1));
	floorMat->SetDiffuse(glm::vec3(0.4));
	floorMat->SetSpecular(glm::vec3(0.4));
	floorMat->SetShininess(0.8);

	floor->SetMaterial(floorMat);
	floor->SetShader("model");
	scene.Add(floor);


	Shared<Model> floorSurface = Model::Create("floorSurface", Primitives::CreateRectangle(300,200));
	floorSurface->Translate(glm::vec3(0,0,0.1));

	Shared<Material> floorMat2 = CreateShared<Material>("floorMat2");
	floorMat2->SetAmbient(glm::vec3(0.4));
	floorMat2->SetDiffuse(glm::vec3(0.9));
	floorMat2->SetSpecular(glm::vec3(0.8));
	floorMat2->SetShininess(0.8);
	floorMat2->LoadTexture("assets/textures/bed.png", TextureType::COLOR);

	floorSurface->SetMaterial(floorMat2);
	floorSurface->SetShader("model");
	scene.Add(floorSurface);




	//Box
	Shared<Model> box = Model::Create("box", Primitives::CreateCube(2, 1.5, 1.5));
	//box->Translate(glm::vec3(0, 0, 0));
	box->SetMaterial("default");
	box->SetShader(modelShader);
	box->EnableWireFrameMode();
	//scene.Add(box);
}

void ExampleLayer::InitPhysics() {
	//Compute Shaders
	init = ComputeShader::Create("init", "assets/shaders/init.comp");
	solver = ComputeShader::Create("solver", "assets/shaders/solver.comp");

	//Particle System
	float gridWidth = 1.0f;

	//Create particle system
	particleSystem = CreateShared<ParticleSystem>("ParticleSystem", maxParticlesCount);
	particleSystem->Translate(glm::vec3(-1, -0.75, -0.71));

	//Define the mesh for instancing (Here a cube)
	//Shared<Mesh> particle = Primitives::CreateCube(0.05 * 0.15);
	//Shared<Mesh> particle = Primitives::CreateSphere(0.05*0.15, 10, 10);
	Shared<Mesh> particle = Primitives::CreatePoint();
	particle->SetName("particle");
	particle->SetShader(particleShader);
	particleSystem->SetMesh(particle);

	//scene.Add(particleSystem);
	//Create the buffer
	particleBuffer = CreateShared<SSBO>("ParticleBuffer");
	particleBuffer->SetBindingPoint(1);
	particleBuffer->Allocate<FluidParticle>(maxParticlesCount);

	binBuffer = CreateShared<SSBO>("BinBuffer");
	binBuffer->SetBindingPoint(2);
	binBuffer->Allocate<Bin>(binCount);
	
	particleSystem->AddStorageBuffer(binBuffer);
	particleSystem->AddStorageBuffer(particleBuffer);
	particleSystem->AddComputeShader(solver);

	scene.Add(particleSystem);

	solver->Use();
	solver->SetUInt("numParticles", numParticles);

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


void ExampleLayer::ResetSimulation() {

	particleSystem->SetActiveInstancesCount(maxParticlesCount);
	particleSystem->Execute(init); //init position using init compute shader
	numParticles = 0;
	solver->Use();
	solver->SetUInt("numParticles", maxParticlesCount);
}


void ExampleLayer::Simulate(Merlin::Timestep ts) {

	solver->Use();

	particleSystem->SetActiveInstancesCount(numParticles);

	particleBuffer->Bind();
	particleBuffer->Attach(*solver);
	binBuffer->Bind();
	binBuffer->Attach(*solver);

	solver->SetFloat("speed", sim_speed);

	solver->SetUInt("stage", 0);
	particleSystem->Execute(solver, false); //Predict
	
	//binBuffer->Bind();
	binBuffer->Clear(); //Clear neighbor list
	solver->SetUInt("stage", 1);
	particleSystem->Execute(solver, false); //Neighbor
	
	
 	for (int i = 0; i < solver_iteration; i++) {

		solver->SetUInt("stage", 2);
		particleSystem->Execute(solver, false); //Compute lambda

		solver->SetUInt("stage", 3);
		particleSystem->Execute(solver, false); //Position delta
	}

	solver->SetUInt("stage", 4);
	particleSystem->Execute(solver, false); //Apply changes
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

	particleSystem->SetThread(thread);
	ResetSimulation();
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
	cameraController->OnUpdate(ts);
	updateFPS(ts);

	if (!paused) {
		Simulate(ts);
	}

	renderer.Clear();
	heatMap->Bind();
	renderer.RenderScene(scene, *camera);
}

void ExampleLayer::OnImGuiRender()
{
	ImGui::Begin("Infos");

	model_camera_translation = camera->GetPosition();
	model_camera_rotation = camera->GetRotation();
	camera_speed = cameraController->GetCameraSpeed();

	ImGui::LabelText("Particle count", std::to_string(numParticles).c_str());

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
		ResetSimulation();
	}

	ImGui::DragInt("Solver iteration", &solver_iteration, 0.1, 1, 50);

	if (ImGui::DragFloat3("Camera position", &model_camera_translation.x, -100.0f, 100.0f)) {
		camera->SetPosition(model_camera_translation);
	}

	if (ImGui::DragFloat3("Camera rotation", &model_camera_rotation.x, -100.0f, 100.0f)) {
		camera->SetRotation(model_camera_rotation);
	}

	if (ImGui::SliderFloat("Camera speed", &camera_speed, 0.0, 50.0f)) {
		cameraController->SetCameraSpeed(camera_speed);
	}
	if (ImGui::SliderFloat("Simulation speed", &sim_speed, 0.0, 5.0f)) {
	}


	ImGui::End();
}


