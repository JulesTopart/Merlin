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
	camera->setNearPlane(0.8f);
	camera->setFarPlane(3000.0f);
	camera->setFOV(60); //Use 90.0f as we are using cubemaps
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
	modelShader->SetVec3("lightPos", glm::vec3(0, 0, 100));

	particleShader = Shader::Create("particle", "assets/shaders/particle.vert", "assets/shaders/particle.frag");
	particleShader->noTexture();
	particleShader->noMaterial();
	particleShader->SetUInt("colorCount", 5);
	particleShader->SetVec4("lightColor", glm::vec4(1));
	particleShader->SetVec3("lightPos", glm::vec3(0, 0, 100));

	renderer.AddShader(modelShader);
	renderer.AddShader(particleShader);

	//renderer.EnableTransparency();
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



	Shared<Model> floor = ModelLoader::LoadModel("./assets/models/bed.stl");
	floor->Translate(glm::vec3(0, 0, -0.2));

	Shared<Material> floorMat = CreateShared<Material>("floorMat");
	floorMat->SetAmbient(glm::vec3(0.1));
	floorMat->SetDiffuse(glm::vec3(0.6));
	floorMat->SetSpecular(glm::vec3(0.2));
	floorMat->SetShininess(0.8);

	floor->SetMaterial("chrome");
	floor->SetShader("model");
	scene.Add(floor);


	Shared<Model> floorSurface = Model::Create("floorSurface", Primitives::CreateRectangle(300, 200));
	floorSurface->Translate(glm::vec3(0, 0, -0.025));

	Shared<Material> floorMat2 = CreateShared<Material>("floorMat2");
	floorMat2->SetAmbient(glm::vec3(0.4));
	floorMat2->SetDiffuse(glm::vec3(0.9));
	floorMat2->SetSpecular(glm::vec3(0.2));
	floorMat2->SetShininess(0.8);
	floorMat2->LoadTexture("assets/textures/bed.png", TextureType::COLOR);

	floorSurface->SetMaterial(floorMat2);
	floorSurface->SetShader("model");
	scene.Add(floorSurface);



	//Box
	Shared<Model> box = Model::Create("box", Primitives::CreateCube(60, 60, 60));
	box->Translate(glm::vec3(0, 0, 30));
	box->SetMaterial("default");
	box->SetShader(modelShader);
	box->EnableWireFrameMode();
	scene.Add(box);
}

void ExampleLayer::InitPhysics() {
	//Compute Shaders
	init = ComputeShader::Create("init", "assets/shaders/init.comp");
	solver = ComputeShader::Create("solver", "assets/shaders/solver.comp");

	//Particle System
	float gridWidth = 1.0f;

	//Create particle system
	particleSystem = CreateShared<ParticleSystem>("ParticleSystem", maxParticlesCount);
	particleSystem->Translate(glm::vec3(0, 0, 0));

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

const long spawnCount = 4 * 4 * 4;
long spawnDelay = 7;//iteration

long timer = 0;
long lastSpawn = 0;

float layerHeight = 0.2;
float firstlayerHeight = 0.1;
glm::vec3 u(0.0,0.0, firstlayerHeight);
const float speed = 1.0;
glm::vec3 v(speed, 0, 0);
int line = 0;
int lineCount = 5;





void snake() {
	if (u.x + v.x >= 0.75*10.0 || u.x <= 0) {
		v.x = 0;
		v.y = speed;
	}

	if (u.y + v.y > (line + 1) * (0.75 * 10.0 / lineCount)) {
		line++;
		v.x = ((line % 2 == 0) ? speed : -speed);
		v.y = 0;
	}

	if (u.y + v.y >= 0.75 * 10.0) {
		line = 0;
		u.y = 0;
		u.x = speed;
		v.y = 0;
		v.x = speed;
		u += glm::vec3(0, 0, (layerHeight));
	}


	u += v;
}


float theta = 0.0;
float radius = 10;
float segment = 10.0 * 36.0;
float theta_v = (2.0 * 3.14159265359 / segment);// *(speed / radius);

void circle() {
	u = glm::vec3((1.0 + cos(theta)) * radius, (1.0 + sin(theta)) * radius, u.z);
	theta += theta_v;
	if (theta >= 2.0*3.14159265359) {
		theta -= 2.0*3.14159265359;
		u.z += layerHeight;
	}
}


void ExampleLayer::ResetSimulation() {

	particleSystem->SetActiveInstancesCount(maxParticlesCount);
	particleSystem->Execute(init); //init position using init compute shader
	numParticles = 0;
	solver->Use();
	solver->SetUInt("numParticles", numParticles);
	u = glm::vec3(0.0, 0.0, firstlayerHeight);
	v = glm::vec3(speed, 0, 0);
	line = 0;
	timer = 0;
	lastSpawn = 0;
	if (sim == 0) spawnDelay = 7;//iteration
	else spawnDelay = 5;//iteration
}


void ExampleLayer::Simulate(Merlin::Timestep ts) {

	timer ++;
	
	if (timer - lastSpawn > spawnDelay/sim_speed) {
		numParticles += spawnCount;
		lastSpawn = timer;
	}

	if(sim == 0)circle();
	else if(sim == 1)snake();


	solver->Use();
	solver->SetVec3("sourcePos", u);




	
	particleSystem->SetActiveInstancesCount(64);

	particleBuffer->Bind();
	particleBuffer->Attach(*solver);
	binBuffer->Bind();
	binBuffer->Attach(*solver);

	solver->SetFloat("speed", sim_speed);

	solver->SetUInt("stage", 0);
	particleSystem->Execute(solver, false); //Predict


	solver->SetUInt("numParticles", numParticles);


	
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

	model_matrix_translation = camera->GetPosition();
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
		//count = 1;
	}

	ImGui::DragInt("Solver iteration", &solver_iteration, 0.1, 1, 50);

	if (ImGui::DragFloat3("Camera position", &model_matrix_translation.x, -100.0f, 100.0f)) {
		camera->SetPosition(model_matrix_translation);
	}
	if (ImGui::SliderFloat("Camera speed", &camera_speed, 0.0, 50.0f)) {
		cameraController->SetCameraSpeed(camera_speed);
	}
	if (ImGui::SliderFloat("Simulation speed", &sim_speed, 0.0, 5.0f)) {
	}

	if (ImGui::Button("Circular Path")) {
		sim = 0;
		ResetSimulation();
	}

	if (ImGui::Button("Snake Path")) {
		sim = 1;
		ResetSimulation();
	}


	ImGui::End();
}


