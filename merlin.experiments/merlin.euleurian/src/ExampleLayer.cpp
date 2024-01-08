#include "ExampleLayer.h"

using namespace Merlin;
using namespace Merlin::Utils;
using namespace Merlin::Graphics;

#include <iostream>
#include <iomanip>

GLsizeiptr gridResolution = 128;
GLsizeiptr gridSize = gridResolution * gridResolution * gridResolution;

ExampleLayer::ExampleLayer() {
	Window* w = &Application::Get().GetWindow();
	int height = w->GetHeight();
	int width = w->GetWidth();
	camera = CreateShared<Camera>(width, height, Projection::Perspective);
	camera->setNearPlane(0.001f);
	camera->setFarPlane(3000.0f);
	camera->setFOV(45.0f); //Use 90.0f as we are using cubemaps
	camera->SetPosition(glm::vec3(-40.0f, 0.0f, 4.0f));
	camera->SetRotation(glm::vec3(0, 0, 0));
	cameraController = CreateShared<CameraController3D>(camera);
}

ExampleLayer::~ExampleLayer(){}

void ExampleLayer::InitGraphics() {
	EnableGLDebugging();
	Window* wd = &Application::Get().GetWindow();
	_height = wd->GetHeight();
	_width = wd->GetWidth();

	Console::SetLevel(ConsoleLevel::_INFO);

	// Init OpenGL stuff
	renderer.Initialize();
	renderer.SetBackgroundColor(0.4, 0.4, 0.4, 1.0);

	//Shaders
	Shared<Shader> modelShader = std::make_shared<Shader>("model", "assets/shaders/model.vert.glsl", "assets/shaders/model.frag.glsl");
	modelShader->Use();
	modelShader->SetVec3("lightPos", glm::vec3(0, 0, 20));

	Shared<Shader> floorShader = std::make_shared<Shader>("floor", "assets/shaders/floor.vert.glsl", "assets/shaders/floor.frag.glsl");
	floorShader->Use();
	floorShader->noTexture();
	//floorShader->noMaterial();
	floorShader->SetVec3("lightPos", glm::vec3(0, 0, 1));

	Shared<Shader> streamlinesShader = std::make_shared<Shader>("streamlines", "assets/shaders/streamlines.vert.glsl", "assets/shaders/streamlines.frag.glsl");
	streamlinesShader->noTexture();
	streamlinesShader->noMaterial();
	streamlinesShader->Use();
	streamlinesShader->SetUInt("colorCount", 5);
	streamlinesShader->SetFloat("grid", float(gridResolution));
	streamlinesShader->SetFloat("gridSpacing", float(20.0f / gridResolution));

	renderer.AddShader(modelShader);
	renderer.AddShader(floorShader);
	renderer.AddShader(streamlinesShader);

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
	//scene.Add(Model::Create("sphere", Primitives::CreateSphere(0.8, 60, 60)));

	Shared<Model> floor = Model::Create("floor", Primitives::CreateFloor(1000, 1.5));
	floor->EnableWireFrameMode();
	floor->Translate(glm::vec3(0, 0, -(0.75 + 1.0 / 32.0)));

	Shared<Material> floorMat = CreateShared<Material>("floorMat");
	floorMat->SetAmbient(glm::vec3(1));
	floorMat->SetDiffuse(glm::vec3(0));
	floorMat->SetSpecular(glm::vec3(0));
	floorMat->SetShininess(0.5);

	floor->SetMaterial(floorMat);
	floor->SetShader("floor");
	scene.Add(floor);

	Shared<Mesh> streamMesh = Primitives::CreateLine(1.0f, glm::vec3(1.0, 0, 0));
	streamMesh->SetDrawMode(GL_LINES);
	streamMesh->SetShader("streamlines");

	streamlines = deprecated_ParticleSystem::Create("StreamLineSystem", 1);
	streamlines->SetMesh(streamMesh);

	scene.Add(streamlines);
}

void ExampleLayer::InitPhysics() {

	//Compute Shaders
	init = std::make_shared<ComputeShader>("init");
	init->Compile("assets/kernels/eulerian.init.glsl");
	init->Use();
	init->SetUInt("grid", gridResolution);

	physics = std::make_shared<ComputeShader>("physics");
	physics->Compile("assets/kernels/eulerian.update.glsl");
	physics->Use();
	physics->SetUInt("grid", gridResolution);
	physics->SetFloat("h", 20.0f / float(gridResolution));

	grid = SSBO<FieldProperty>::Create("FieldBuffer");
	grid->Allocate(gridSize);

	newGrid = SSBO<FieldProperty>::Create("NewFieldBuffer");
	newGrid->Allocate(gridSize);

	streamlines->AddComputeShader(physics);
	streamlines->AddStorageBuffer(grid);
	streamlines->AddStorageBuffer(newGrid);
	streamlines->SetInstancesCount(128);

}

void ExampleLayer::SetColorGradient() {

	std::vector<Color> colors;
	colors.push_back({ glm::vec3(0, 0, 1), 0.0f });     // Blue.
	colors.push_back({ glm::vec3(0, 1, 1), 0.25f });     // Cyan.
	colors.push_back({ glm::vec3(0, 1, 0), 0.5f });     // Green.
	colors.push_back({ glm::vec3(1, 1, 0), 0.75f });     // Yellow.
	colors.push_back({ glm::vec3(1, 0, 0), 1.0f });     // Red.
	colorCount = colors.size();

	heatMap = SSBO<Color>::Create("ColorMapBuffer");
	heatMap->LoadData(colors);
}


void ExampleLayer::OnAttach(){
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

	cameraController->OnUpdate(ts);
	updateFPS(ts);

	if(!paused){
		physics->Use();
		physics->SetFloat("dt", ts);
		physics->SetFloat("speed", sim_speed);
		//newGrid->BindAs(GL_COPY_READ_BUFFER);
		//grid->BindAs(GL_COPY_WRITE_BUFFER);
		//glCopyNamedBufferSubData(newGrid->id(), grid->id(), 0, 0, gridSize);
		//glMemoryBarrier(GL_ALL_BARRIER_BITS);
	}


	renderer.Clear();
	heatMap->Bind();
	renderer.RenderScene(scene, *camera);
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


