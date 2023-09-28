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

ExampleLayer::~ExampleLayer() {}

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

	binShader = Shader::Create("bins", "assets/shaders/bin.vert", "assets/shaders/bin.frag");
	binShader->noTexture();
	binShader->noMaterial();
	binShader->SetUInt("colorCount", 5);
	binShader->SetVec4("lightColor", glm::vec4(1));
	binShader->SetVec3("lightPos", glm::vec3(0, 0, 100));

	renderer.AddShader(modelShader);
	renderer.AddShader(particleShader);
	renderer.AddShader(binShader);

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
	Shared<Model> box = Model::Create("box", Primitives::CreateQuadCube(settings.bx, settings.by, settings.bz));
	box->Translate(glm::vec3(0, 0, settings.bz/2.0));
	box->SetMaterial("default");
	box->SetShader(modelShader);
	box->EnableWireFrameMode();
	scene.Add(box);

	nozzleMdl = ModelLoader::LoadModel("./assets/models/nozzle.stl");
	nozzleMdl->SetMaterial("gold");
	nozzleMdl->SetShader(modelShader);
	nozzleMdl->Rotate(glm::vec3(3.141592654, 0, 0));
	nozzleMdl->Translate(glm::vec3(0, 0, -20));
	nozzleMdl->Scale(0.10);

	nozzle = TransformObject::Create("nozzleTransform");
	nozzle->AddChild(nozzleMdl);
	nozzle->Translate(glm::vec3(0, 0, 0));
	scene.Add(nozzle);

}

void ExampleLayer::InitPhysics() {
	//Compute Shaders
	init = CreateShared<ComputeShader>("init", "assets/shaders/init.comp");
	solver = CreateShared<StagedComputeShader>("solver", "assets/shaders/solver.comp", 6);
	prefixSum = CreateShared<StagedComputeShader>("prefixSum", "assets/shaders/prefix.sum.comp", 4);
	init->SetWorkgroupLayout(settings.pWkgCount);
	solver->SetWorkgroupLayout(settings.pWkgCount);
	prefixSum->SetWorkgroupLayout(settings.bwgCount);

	
	//Create particle system
	particleSystem = ParticleSystem<FluidParticle>::Create("ParticleSystem", settings.pThread);
	particleSystem->Translate(glm::vec3(0, 0, 0));

	//Define the mesh for instancing (Here a cube)
	//Shared<Mesh> particle = Primitives::CreateCube(0.05 * 0.15);
	//Shared<Mesh> particle = Primitives::CreateSphere(0.05*0.15, 10, 10);
	Shared<Mesh> particle = Primitives::CreatePoint();
	particle->Rename("particle");
	particle->SetShader(particleShader);
	particleSystem->SetMesh(particle);

	//Create bin system
	binSystem = ParticleSystem<FluidParticle>::Create("BinSystem", settings.bThread);
	binSystem->Translate(glm::vec3(0, 0, 0));

	//Define the mesh for bin instancing (Here a cube)
	Shared<Mesh> binInstance = Primitives::CreateQuadCube(settings.bWidth, false);
	binInstance->Rename("bin");
	binInstance->SetShader(binShader);
	binSystem->SetMesh(binInstance);
	binSystem->EnableWireFrameMode();

	//scene.Add(particleSystem);
	//Create the buffer
	particleBuffer = SSBO<FluidParticle>::Create("ParticleBuffer");
	particleBuffer->Allocate(settings.pThread);

	binBuffer = SSBO<Bin>::Create("BinBuffer");
	binBuffer->Allocate(settings.bThread);

	particleSystem->AddComputeShader(solver);
	particleSystem->AddStorageBuffer(particleBuffer);
	particleSystem->AddStorageBuffer(binBuffer);
	
	binSystem->AddComputeShader(prefixSum);
	binSystem->AddStorageBuffer(binBuffer);

	scene.Add(particleSystem);
	scene.Add(binSystem);
	binSystem->Hide();

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

	heatMap = SSBO<Color>::Create("ColorMapBuffer");
	heatMap->LoadData(colors);
}

const long lCount = 4;
const long spawnCount = lCount * lCount * lCount;
long spawnDelay = 1;//iteration old7

long timer = 0;
long lastSpawn = 0;

float layerHeight = 0.2;
float firstlayerHeight = 0.1;
glm::vec3 u(150.0, 0.0, firstlayerHeight);
const float speed = 2.0; //old 0.8
glm::vec3 v(speed, 0, 0);
int line = 0;
int lineCount = 5;
glm::vec3 partDim(50, 50, 50);


glm::vec3 snake() {
	if (u.x + v.x >= 0.9 * partDim.x || u.x <= 0) {
		v.x = 0;
		v.y = speed;
	}

	if (u.y + v.y > (line + 1) * (0.75 * partDim.y / lineCount)) {
		line++;
		v.x = ((line % 2 == 0) ? speed : -speed);
		v.y = 0;
	}

	if (u.y + v.y >= 0.75 * partDim.y) {
		line = 0;
		u.y = 0;
		u.x = speed;
		v.y = 0;
		v.x = speed;
		u += glm::vec3(0, 0, (layerHeight));
	}


	u += v;
	return u;
}


float theta = 0.0;
float radius = 25;
float segment = 10.0 * 36.0;
float theta_v = (speed * 3.14159265359 / segment);// *(speed / radius);

glm::vec3 circle() {
	u = glm::vec3((cos(theta)) * radius, (sin(theta)) * radius, u.z);
	theta += theta_v;
	if (theta >= 2.0 * 3.14159265359) {
		theta -= 2.0 * 3.14159265359;
		u.z += layerHeight;
	}
	return u;
}


void ExampleLayer::ResetSimulation() {

	particleSystem->SetInstancesCount(settings.pThread);
	init->Dispatch(); //init position using init compute shader
	numParticles = 0;
	solver->Use();
	solver->SetUInt("numParticles", numParticles);
	u = glm::vec3(0.0, 0.0, firstlayerHeight);
	v = glm::vec3(speed, 0, 0);
	line = 0;
	timer = 0;
	lastSpawn = 0;
	if (sim == 0) spawnDelay = 1;//iteratinon
	else spawnDelay = 5;//iteratinon
}


glm::uvec3 getBinCoord(glm::vec3 position) {
	position *= settings.scale;
	position += glm::vec3(150,100,125);
	glm::uvec3 bin3D = glm::uvec3(position / settings.bWidth);
	bin3D.x = max(min(bin3D.x, (settings.bx / (settings.bWidth)) - 1), 0);
	bin3D.y = max(min(bin3D.y, (settings.by / (settings.bWidth)) - 1), 0);
	bin3D.z = max(min(bin3D.z, (settings.bz / (settings.bWidth)) - 1), 0);
	return bin3D;
}

GLuint getBinIndexFromCoord(glm::uvec3 coord) {
	return (coord.z * (settings.bx / (settings.bWidth)) * (settings.by / (settings.bWidth))) + (coord.y * (settings.bx / (settings.bWidth))) + coord.x;
}

GLuint getBinIndex(glm::vec3 position) {
	glm::uvec3 bin3D = getBinCoord(position);
	return getBinIndexFromCoord(bin3D);
}

glm::uvec3 getBinCoordFromIndex(GLuint index) {
	GLuint z = index / ((settings.bx / (settings.bWidth)) * (settings.by / (settings.bWidth)));
	index -= (z * (settings.bx/ (settings.bWidth)) * (settings.by / (settings.bWidth)));
	GLuint y = index / (settings.bx / (settings.bWidth));
	GLuint x = index % int(settings.bx / (settings.bWidth));
	return glm::uvec3(x, y, z);
}



void ExampleLayer::Simulate(Merlin::Timestep ts) {

	timer++;
	solver->Use();

	if (timer - lastSpawn > spawnDelay / sim_speed) {
		numParticles += spawnCount;
		particleSystem->SetInstancesCount(numParticles);
		lastSpawn = timer;
	}

	if (sim == 0)nozzle->SetPosition(circle());
	else if (sim == 1)nozzle->SetPosition(snake());

	solver->SetVec3("sourcePos", u);
	solver->SetFloat("speed", sim_speed);
	solver->SetUInt("numParticles", numParticles); //Spawn particle after prediction

	solver->Execute(0); //Predict
	binBuffer->Clear(); //Reset neighbor search data
	solver->Execute(1); //Place particles in bins

	prefixSum->Use();
	prefixSum->SetUInt("dataSize", settings.pThread); //data size
	prefixSum->SetUInt("blockSize", settings.blockSize); //block size

	prefixSum->Execute(0);// local prefix sum

	//Binary tree on rightmost element of blocks
	GLuint steps = settings.blockSize;
	UniformObject<GLuint> space("space");
	space.value = 1;

	for (GLuint step = 0; step < steps; step++) {
		// Calls the parallel operation

		space.Sync(*prefixSum);
		prefixSum->Execute(1);
		prefixSum->Execute(2);

		space.value *= 2;
	}
	prefixSum->Execute(3);

	solver->Use();
	solver->Execute(2); //Sort
	
	for (int i = 0; i < solver_iteration; i++) {
		solver->Execute(3); //Compute lambda
		solver->Execute(4); //Position delta
	}
	solver->Execute(5); //Apply changes

	/*
	binBuffer->Sync();
	particleBuffer->Sync();
	particleBuffer->Sync();
	*/

}

void ExampleLayer::OnAttach() {
	EnableGLDebugging();
	Window* wd = &Application::Get().GetWindow();
	_height = wd->GetHeight();
	_width = wd->GetWidth();

	Console::SetLevel(ConsoleLevel::_TRACE);

	InitGraphics();
	InitPhysics();
	SetColorGradient();


	ResetSimulation();
}

void ExampleLayer::OnDetach() {}

void ExampleLayer::updateFPS(Timestep ts) {
	if (FPS_sample == 0) {
		FPS = ts;
	}
	else {
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
	
	renderer.RenderScene(scene, *camera);
}

void ExampleLayer::OnImGuiRender()
{
	ImGui::Begin("Infos");

	model_matrix_translation = camera->GetPosition();
	camera_speed = cameraController->GetCameraSpeed();

	ImGui::LabelText(std::to_string(numParticles).c_str(), "particles");
	ImGui::LabelText(std::to_string(settings.bThread).c_str(), "bins");

	if (FPS_sample > 0) {
		ImGui::LabelText("FPS", std::to_string(1.0f / (FPS / FPS_sample)).c_str());
		if (FPS_sample > 50) FPS_sample = 0;
	}

	if (paused) {
		if (ImGui::ArrowButton("Run simulation", 1)) {
			paused = !paused;
		}
	}
	else {
		if (ImGui::SmallButton("Pause simulation")) {
			paused = !paused;
			particleBuffer->print();
		}
	}

	if (ImGui::SmallButton("Reset simulation")) {
		ResetSimulation();
		//count = 1;
	}

	static bool Pstate = true;
	if(ImGui::Checkbox("Show Particles", &Pstate)) {
		if(Pstate) particleSystem->Show();
		else particleSystem->Hide();
	}

	static bool Bstate = false;
	if (ImGui::Checkbox("Show Bins", &Bstate)) {
		if (Bstate) binSystem->Show();
		else binSystem->Hide();
	}

	static bool Cstate = false;
	if (ImGui::Checkbox("Color cycling", &Cstate)) {
		particleShader->Use();
		particleShader->SetInt("colorCycle", Cstate ? 1 : 0);
		binShader->Use();
		binShader->SetInt("colorCycle", Cstate ? 1 : 0);
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