#include "ExampleLayer.h"

using namespace Merlin;
using namespace Merlin::Utils;
using namespace Merlin::Graphics;

#include <iostream>
#include <iomanip>

#include "Bunny.h"

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

	particleShader->Use();
	particleShader->SetInt("colorCycle", 1);
	binShader->Use();
	binShader->SetInt("colorCycle", 1);

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
	floor->Translate(glm::vec3(0, 0, -0.1));

	Shared<Material> floorMat = CreateShared<Material>("floorMat");
	floorMat->SetAmbient(glm::vec3(0.1));
	floorMat->SetDiffuse(glm::vec3(0.6));
	floorMat->SetSpecular(glm::vec3(0.2));
	floorMat->SetShininess(0.8);

	floor->SetMaterial("chrome");
	floor->SetShader("model");
	scene.Add(floor);

	Shared<Model> floorSurface = Model::Create("floorSurface", Primitives::CreateRectangle(300, 200));
	//floorSurface->Translate(glm::vec3(0, 0, 0));

	Shared<Material> floorMat2 = CreateShared<Material>("floorMat2");
	floorMat2->SetAmbient(glm::vec3(0.4));
	floorMat2->SetDiffuse(glm::vec3(0.9));
	floorMat2->SetSpecular(glm::vec3(0.2));
	floorMat2->SetShininess(0.8);
	floorMat2->LoadTexture("assets/textures/bed.png", TextureType::COLOR);

	floorSurface->SetMaterial(floorMat2);
	floorSurface->SetShader("model");
	scene.Add(floorSurface);
	 
	/*
	Shared<Model> bunny = ModelLoader::LoadModel("./assets/models/bunny.stl");
	bunny->Translate(glm::vec3(-1, -4, -0.7));
	bunny->Scale(0.118);
	bunny->Rotate(glm::vec3(0,0,3.1415926/2));
	bunny->SetMaterial("jade");
	bunny->SetShader("model");
	scene.Add(bunny);*/

	//Box
	Shared<Model> box = Model::Create("box", Primitives::CreateQuadCube(settings.bx, settings.by, settings.bz));
	box->Translate(glm::vec3(0, 0, settings.bz/2.0));
	box->SetMaterial("default");
	box->SetShader(modelShader);
	box->EnableWireFrameMode();
	scene.Add(box);


	origin = TransformObject::Create("origin");
	scene.Add(origin);

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
	particleSystem->Translate(glm::vec3(0, 0, 0.5));

	//Define the mesh for instancing (Here a cube)
	//Shared<Mesh> particle = Primitives::CreateCube(0.5);
	//Shared<Mesh> particle = Primitives::CreateSphere(0.05*0.15, 10, 10);
	Shared<Mesh> particle = Primitives::CreatePoint();
	particle->Rename("particle");
	particle->SetShader(particleShader);
	particleSystem->SetMesh(particle);
	particleSystem->Translate(glm::vec3(0, 0, -0.5));
	particleSystem->SetDisplayMode(ParticleSystemDisplayMode::POINT_SPRITE);

	//Create bin system
	Console::info("Memory") << "size of FluidParticle is " << sizeof(FluidParticle) << Console::endl;
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
	binSystem->AddStorageBuffer(particleBuffer);
	binSystem->AddStorageBuffer(binBuffer);

	scene.Add(particleSystem);
	scene.Add(binSystem);
	binSystem->Hide();

	solver->Use();
	solver->SetUInt("numParticles", numParticles);
}

void ExampleLayer::SetColorGradient() {
	std::vector<glm::vec4> colors;
	colors.push_back( glm::vec4(0, 0, 1,0.0f));     // Blue.
	colors.push_back( glm::vec4(0, 1, 1,0.25f));     // Cyan.
	colors.push_back( glm::vec4(0, 1, 0,0.5f));     // Green.
	colors.push_back( glm::vec4(1, 1, 0,0.75f));     // Yellow.
	colors.push_back( glm::vec4(1, 0, 0,1.0f));     // Red.
	colorCount = colors.size();

	heatMap = SSBO<glm::vec4>::Create("ColorMapBuffer");
	heatMap->LoadData(colors);
}


void AddSphere() {

}

void ExampleLayer::ResetSimulation() {

	particleSystem->SetInstancesCount(settings.pThread);
	//init->Use();
	//init->Dispatch(); //init position using init compute shader
	particleBuffer->Bind();
	particleBuffer->Clear();
	particleBuffer->FreeHostMemory();

	Console::info() << "Generating particles..." << Console::endl;

	std::vector<glm::vec3> bunny = GenerateVoxelBunny(0.5);
	auto& cpu_particles = particleBuffer->GetDeviceBuffer();

	FluidParticle buf;
	buf.velocity[0] = 0;
	buf.velocity[1] = 0;
	buf.velocity[2] = 0;
	buf.density = 0;

	buf.phase = SOLID; //Rigid body
	for (auto& v : bunny) {
		buf.position[0] = v.x;
		buf.position[1] = v.y;
		buf.position[2] = v.z;
		cpu_particles.push_back(buf);
	}
	

	//Generate sphere above
	buf.phase = LIQUID; //Fluid body
	const float height = 10;
	//const float height = 80;
	const float goldenRatio = (1.0 + sqrt(5.0)) / 2.0;
	const float angleIncrement = 3.1415926 * 2.0 * goldenRatio;
	const int radius = 8;
	for (float r = 0.1; r < radius; r+=0.1)
	for (int i = 0; i < 4*3.1415926 * r * r; ++i) {
		float t = float(i) / float(4 * 3.1415926 * r * r);
		float inclinationAngle = acos(1 - 2 * t);  // polar angle
		float azimuthalAngle = angleIncrement * i;  // azimuthal angle
		buf.position[0] = r*sin(inclinationAngle) * cos(azimuthalAngle);
		buf.position[1] = r*sin(inclinationAngle) * sin(azimuthalAngle);
		buf.position[2] = r*cos(inclinationAngle) + height;
		cpu_particles.push_back(buf);
	}
	
	float spacing = 0.4;
	buf.phase = BOUNDARY; //Boundaries body
	for (float x = -settings.bx / 2.0; x < settings.bx / 2.0; x += spacing) {
		for (float y = -settings.by / 2.0; y < settings.by / 2.0; y += spacing) {
			buf.position[0] = x;
			buf.position[1] = y;
			buf.position[2] = 0;
			cpu_particles.push_back(buf);
			buf.position[2] = settings.bz;
			cpu_particles.push_back(buf);
		}
	}
	for (float y = -settings.by / 2.0; y < settings.by / 2.0; y += spacing) {
		for (float z = 0; z < settings.bz; z += spacing) {
			buf.position[0] = -settings.bx / 2.0;
			buf.position[1] = y;
			buf.position[2] = z;
			cpu_particles.push_back(buf);
			buf.position[0] = settings.bx / 2.0;
			cpu_particles.push_back(buf);
		}
	}
	for (float x = -settings.bx / 2.0; x < settings.bx / 2.0; x += spacing) {
		for (float z = 0; z < settings.bz; z += spacing) {
			buf.position[0] = x;
			buf.position[1] = -settings.by / 2.0;
			buf.position[2] = z;
			cpu_particles.push_back(buf);
			buf.position[1] = settings.by / 2.0;
			cpu_particles.push_back(buf);
		}
	}


	particleBuffer->Upload();
	Console::info() << "Loaded Stanford rabbit and a sphere in particle buffer (" << cpu_particles.size() << " particles )" << Console::endl;


	binBuffer->Bind();
	binBuffer->Clear();

	numParticles = cpu_particles.size();
	particleSystem->SetInstancesCount(numParticles);


	solver->Use();
	solver->SetUInt("numParticles", numParticles);

	// --- SPH ---
	// SPH Parameters
	solver->SetFloat("H",settings.H); // Kernel radius // 5mm
	solver->SetFloat("particleMass",settings.particleMass); // Kernel radius // 5mm
	solver->SetFloat("REST_DENSITY",settings.REST_DENSITY); // Kernel radius // 5mm

	particleShader->Use();
	particleShader->SetUInt("numParticles", numParticles);
}


glm::uvec3 ExampleLayer::getBinCoord(glm::vec3 position) {
	position *= settings.scale;
	position += glm::vec3(150,100,125);
	glm::uvec3 bin3D = glm::uvec3(position / settings.bWidth);
	bin3D.x = max(min(bin3D.x, (settings.bx / (settings.bWidth)) - 1), 0);
	bin3D.y = max(min(bin3D.y, (settings.by / (settings.bWidth)) - 1), 0);
	bin3D.z = max(min(bin3D.z, (settings.bz / (settings.bWidth)) - 1), 0);
	return bin3D;
}

GLuint ExampleLayer::getBinIndexFromCoord(glm::uvec3 coord) {
	return (coord.z * (settings.bx / (settings.bWidth)) * (settings.by / (settings.bWidth))) + (coord.y * (settings.bx / (settings.bWidth))) + coord.x;
}

GLuint ExampleLayer::getBinIndex(glm::vec3 position) {
	glm::uvec3 bin3D = getBinCoord(position);
	return getBinIndexFromCoord(bin3D);
}

glm::uvec3 ExampleLayer::getBinCoordFromIndex(GLuint index) {
	GLuint z = index / ((settings.bx / (settings.bWidth)) * (settings.by / (settings.bWidth)));
	index -= (z * (settings.bx/ (settings.bWidth)) * (settings.by / (settings.bWidth)));
	GLuint y = index / (settings.bx / (settings.bWidth));
	GLuint x = index % int(settings.bx / (settings.bWidth));
	return glm::uvec3(x, y, z);
}


void ExampleLayer::NeigborSearch() {
	prefixSum->Use();
	prefixSum->SetUInt("dataSize", settings.bThread); //data size
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
}

void ExampleLayer::Simulate(Merlin::Timestep ts) {

	solver->Use();
	solver->SetFloat("speed", sim_speed);
	solver->SetUInt("numParticles", numParticles); //Spawn particle after prediction

	binBuffer->Bind();
	binBuffer->Clear(); //Reset neighbor search data
	solver->Execute(0); //Place particles in bins

	NeigborSearch();

	solver->Use();
	solver->Execute(1); //Sort
	solver->Execute(2); //Predict

	for (int i = 0; i < solver_iteration; i++) {
		solver->Execute(3); //Compute lambda
		solver->Execute(4); //Position delta
	}
	solver->Execute(5); //Apply changes
}

void ExampleLayer::OnAttach() {
	EnableGLDebugging();
	Window* wd = &Application::Get().GetWindow();
	_height = wd->GetHeight();
	_width = wd->GetWidth();

	Console::SetLevel(ConsoleLevel::_INFO);

	InitGraphics();
	InitPhysics();
	SetColorGradient();

	particleShader->Attach(*particleBuffer);
	particleShader->Attach(*binBuffer);
	particleShader->Attach(*heatMap);

	binShader->Attach(*particleBuffer);
	binShader->Attach(*binBuffer);
	binShader->Attach(*heatMap);

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
			//particleBuffer->print();
			//binBuffer->print();
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

	static bool BBstate = false;
	if (ImGui::Checkbox("Show Boundaries", &BBstate)) {
		particleShader->Use();
		particleShader->SetInt("showBoundary", BBstate);
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


	if (ImGui::SliderFloat("Smoothing radius", &settings.H, 0.5, 10.0)) {
		solver->Use();
		solver->SetFloat("H", settings.H); // Kernel radius // 5mm
	}

	if (ImGui::SliderFloat("Rest density", &settings.REST_DENSITY, 10, 5000)) {
		solver->Use();
		solver->SetFloat("REST_DENSITY", settings.REST_DENSITY); // Kernel radius // 5mm
	}

	if (ImGui::SliderFloat("mass", &settings.particleMass, 0.0, 10.0f)) {
		solver->Use();
		solver->SetFloat("particleMass", settings.particleMass); // Kernel radius // 5mm
	}

	
	static int colorMode = 1;
	if (ImGui::SliderInt("Color Mode", &colorMode, 0.0, 3.0f)) {
		particleShader->Use();
		particleShader->SetInt("colorCycle", colorMode);
		binShader->Use();
		binShader->SetInt("colorCycle", colorMode);
	}

	ImGui::End();



	ImGui::Begin("Compute Shader");
	ImGui::LabelText("Solver", "");
	static int stepSolver = 0;
	ImGui::SliderInt("Solver Step", &stepSolver, 0.0, 5.0f);
	if (ImGui::Button("Execute Solver")) {
		solver->Use();
		solver->Execute(stepSolver);
	}

	ImGui::LabelText("PrefixSum", "");
	static int stepPrefix = 0;
	ImGui::LabelText("Solver", "");
	ImGui::SliderInt("PrefixSum Step", &stepPrefix, 0.0, 3.0f);
	if (ImGui::Button("Execute PrefixSum Step")) {
		prefixSum->Use();
		prefixSum->Execute(stepPrefix);
	}

	if (ImGui::Button("Step Simulation")) {
		Simulate(0.016);
	}

	if (ImGui::Button("Debug")) {
		particleBuffer->Bind();
		particleBuffer->Download();
		binBuffer->Bind();
		binBuffer->Download();

		Console::info() << "DEBUG" << Console::endl;
	}
	ImGui::End();
}