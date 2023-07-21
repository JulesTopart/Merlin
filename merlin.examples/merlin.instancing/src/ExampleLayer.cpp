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
	camera->setNearPlane(0.001f);
	camera->setFarPlane(3000.0f);
	camera->setFOV(45.0f); //Use 90.0f as we are using cubemaps
	camera->SetPosition(glm::vec3(0.0f, 4.0f, 0.0f));
	camera->SetRotation(glm::vec3(0, 0, -90));
	cameraController = CreateShared<CameraController3D>(camera);
}

ExampleLayer::~ExampleLayer(){}

GLsizeiptr count = 1;


void ExampleLayer::InitGraphics() {
	EnableGLDebugging();
	Window* wd = &Application::Get().GetWindow();
	_height = wd->GetHeight();
	_width = wd->GetWidth();

	Console::SetLevel(ConsoleLevel::_INFO);

	// Init OpenGL stuff
	renderer.Initialize();
	renderer.SetBackgroundColor(0.203, 0.203, 0.203, 1.0);

	//Shaders
	Shared<Shader> modelShader = std::make_shared<Shader>("model", "assets/shaders/model.vert.glsl", "assets/shaders/model.frag.glsl");
	Shared<Shader> particleShader = std::make_shared<Shader>("particle", "assets/shaders/particle.vert.glsl", "assets/shaders/particle.frag.glsl");
	particleShader->noTexture();
	particleShader->noMaterial();

	renderer.AddShader(modelShader);
	renderer.AddShader(particleShader);


	//Model
	Shared<Model> floor = Model::Create("floor", Primitives::CreateCube(4, 2, 0.1));
	floor->Translate(glm::vec3(0, 0, -(0.75 + 1.0 / 32.0)));

	Shared<Material> floorMat = CreateShared<Material>("floorMat");
	floorMat->SetAmbient(glm::vec3(0.2));
	floorMat->SetDiffuse(glm::vec3(0.4));
	floorMat->SetSpecular(glm::vec3(0.2));
	floorMat->SetShininess(0.2);
	floor->SetMaterial(floorMat);
	floor->SetShader("model");
	scene.Add(floor);

	Shared<Model> box = Model::Create("box", Primitives::CreateCube(2, 1.5, 1.5));
	//box->Translate(glm::vec3(0, 0, 0));
	box->SetMaterial("default");
	box->SetShader(modelShader);
	box->EnableWireFrameMode();
	scene.Add(box);


	//SkyBox
	std::vector<std::string> skyBoxPath = {
		"./assets/textures/skybox/right.jpg",
		"./assets/textures/skybox/left.jpg",
		"./assets/textures/skybox/bottom.jpg",
		"./assets/textures/skybox/top.jpg",
		"./assets/textures/skybox/front.jpg",
		"./assets/textures/skybox/back.jpg"
	};

	Shared<Shader> skyShader = Shader::Create("skybox", "assets/shaders/skybox.vert.glsl", "assets/shaders/skybox.frag.glsl");
	Shared<SkyBox> sky = CreateShared<SkyBox>("Sky", skyBoxPath);
	sky->SetShader(skyShader);
	scene.Add(sky);
}

void ExampleLayer::InitPhysics() {
	//Compute Shaders
	init = std::make_shared<ComputeShader>("init");
	init->Compile("assets/shaders/particle.init.glsl");

	physics = std::make_shared<ComputeShader>("physics");
	//physics->Compile("assets/shaders/particle.update.glsl");
	physics->Compile("assets/shaders/particle.sph.glsl");



	//Particle System settings
	GLsizeiptr gridSize = 32;
	GLsizeiptr partCount = gridSize * gridSize * gridSize;
	count = partCount;
	float gridWidth = 1.0f;

	//Create particle system
	particleSystem = CreateShared<ParticleSystem>("ParticleSystem", partCount);
	particleSystem->Translate(glm::vec3(-1, -0.75, -0.71));

	//Define the mesh for instancing (Here a cube)
	//Shared<Mesh> cube = Primitives::CreateCube(0.05f);
	Shared<Mesh> cube = Primitives::CreateSphere(gridWidth / (2.0f * float(gridSize)), 5, 5);
	cube->SetName("particle");
	cube->SetMaterial("cyan plastic");
	//cube->SetShader(modelShader);
	cube->SetShader("particle");
	particleSystem->SetMesh(cube);

	scene.Add(particleSystem);


	//Create the buffer
	Shared<SSBO> buffer = CreateShared<SSBO>("ParticleBuffer");
	buffer->SetBindingPoint(1);
	buffer->Allocate<TVEParticle>(partCount);
	particleSystem->AddStorageBuffer(buffer);
	particleSystem->AddComputeShader(physics);

	init->Use();
	init->SetUInt("grid", gridSize);
	init->SetFloat("gridSpacing", gridWidth / float(gridSize));

	physics->Use();
	physics->SetInt("numParticles", partCount);


	particleSystem->SetThread(128);
	particleSystem->Execute(init);
}

void ExampleLayer::OnAttach(){
	InitGraphics();
	InitPhysics();	
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
	physics->Use();
	physics->SetFloat("speed", sim_speed);

	if (!paused) {
		particleSystem->Update(0.016);
	}

	renderer.Clear();
	renderer.RenderScene(scene, *camera);

}

void ExampleLayer::OnImGuiRender()
{
	ImGui::Begin("Infos");

	model_matrix_translation = camera->GetPosition();
	camera_speed = cameraController->GetCameraSpeed();

	ImGui::LabelText("Particle count", std::to_string(count).c_str());

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
		particleSystem->Execute(init); //init position using init compute shader
		count = 1;
	}
	if (ImGui::DragFloat3("Camera position", &model_matrix_translation.x, -100.0f, 100.0f)) {
		camera->SetPosition(model_matrix_translation);
	}
	if (ImGui::SliderFloat("Camera speed", &camera_speed, 0.0, 50.0f)) {
		cameraController->SetCameraSpeed(camera_speed);
	}
	if (ImGui::SliderFloat("Simulation speed", &sim_speed, 0.0, 2.0f)) {
	}


	ImGui::End();
}
