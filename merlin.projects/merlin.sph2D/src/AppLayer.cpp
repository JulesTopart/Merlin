#include "AppLayer.h"

#include <GLFW/glfw3.h>
#include <iostream>
#include <iomanip>

using namespace Merlin;
using namespace Merlin::Utils;
using namespace Merlin::Memory;
using namespace Merlin::Graphics;

#define PROFILE(VAR, CODE) double start_ ## VAR ## _time = glfwGetTime(); CODE VAR = (glfwGetTime() - start_ ## VAR ## _time)*1000.0;
#define GPU_PROFILE(VAR, CODE) double start_ ## VAR ## _time = glfwGetTime(); CODE glFinish(); VAR = (glfwGetTime() - start_ ## VAR ## _time)*1000.0;

#define PROFILE_BEGIN(STARTVAR) STARTVAR = glfwGetTime();
#define PROFILE_END(STARTVAR, VAR) VAR = (glfwGetTime() - STARTVAR)*1000.0

AppLayer::AppLayer(){
	Window* w = &Application::Get().GetWindow();
	int height = w->GetHeight();
	int width = w->GetWidth();
	camera = CreateShared<Camera>(width, height, Projection::Orthographic);
	camera->setNearPlane(0.0f);
	camera->setFarPlane(10.0f);
	camera->Translate(glm::vec3(0, 0, 1));
	cameraController = CreateShared<CameraController2D>(camera);
	cameraController->SetZoomLevel(20);
	cameraController->SetCameraSpeed(100);
}

AppLayer::~AppLayer(){}

void AppLayer::OnAttach(){
	EnableGLDebugging();
	//ImGui::LoadIniSettingsFromDisk("imgui.ini");
	Console::SetLevel(ConsoleLevel::_INFO);

	InitGraphics();
	InitPhysics();

	particleShader->Use();
	particleShader->Attach(*particleBuffer);
	particleShader->Attach(*sortedIndexBuffer);
	particleShader->Attach(*binBuffer);

	binShader->Use();
	binShader->Attach(*particleBuffer);
	binShader->Attach(*sortedIndexBuffer);
	binShader->Attach(*binBuffer);

	ResetSimulation();
}

void AppLayer::OnDetach(){}

void AppLayer::OnEvent(Event& event){
	camera->OnEvent(event);
	cameraController->OnEvent(event);

	if (event.GetEventType() == EventType::MouseScrolled) {
		particleShader->Use();
		particleShader->SetFloat("zoomLevel", camera->GetZoom());
	}
}

float t = 0.0;

void AppLayer::OnUpdate(Timestep ts){
	cameraController->OnUpdate(ts);
	PROFILE_END(total_start_time, total_time);
	PROFILE_BEGIN(total_start_time);

	updateFPS(ts);

	if (!paused) {
		GPU_PROFILE(solver_total_time,
			Simulate(0.016);
		)
	}

	
	GPU_PROFILE(render_time,
		renderer.Clear();
		renderer.RenderScene(scene, *camera);
	)
}



void AppLayer::UpdateBufferSettings() {
	settings.pWkgCount = (settings.pThread + settings.pWkgSize - 1) / settings.pWkgSize; //Total number of workgroup needed
	settings.bWidth = std::max(settings.bx, settings.by) / float(settings.bRes); //Width of a single bin in mm
	settings.bThread = int(settings.bx / (settings.bWidth)) * int(settings.by / (settings.bWidth)); //Total number of bin (thread)
	settings.blockSize = floor(log2f(settings.bThread));
	settings.blocks = (settings.bThread + settings.blockSize - 1) / settings.blockSize;
	settings.bWkgCount = (settings.blocks + settings.bWkgSize - 1) / settings.bWkgSize; //Total number of workgroup needed

	solver->SetWorkgroupLayout(settings.pWkgCount);
	prefixSum->SetWorkgroupLayout(settings.bWkgCount);

	particleBuffer->Bind();
	particleBuffer->Resize(settings.pThread);

	particleCpyBuffer->Bind();
	particleCpyBuffer->Resize(settings.pThread);

	sortedIndexBuffer->Bind();
	sortedIndexBuffer->Resize(settings.pThread);

	binBuffer->Bind();
	binBuffer->Resize(settings.bThread);
}

void AppLayer::InitGraphics() {
	// Init OpenGL stuff
	renderer.Initialize();
	renderer.SetBackgroundColor(0.203, 0.203, 0.203, 1.0);
	renderer.EnableTransparency();
	renderer.EnableSampleShading();

	particleShader = Shader::Create("particle", "assets/shaders/particle.vert", "assets/shaders/particle.frag");
	particleShader->noTexture();
	particleShader->noMaterial();
	particleShader->SetVec3("lightPos", glm::vec3(0, -200, 1000));

	binShader = Shader::Create("bins", "assets/shaders/bin.vert", "assets/shaders/bin.frag");
	binShader->noTexture();
	binShader->noMaterial();

	particleShader->Use();
	particleShader->SetInt("colorCycle", 0);
	binShader->Use();
	binShader->SetInt("colorCycle", 0);
	 
	renderer.AddShader(particleShader);
	renderer.AddShader(binShader);

	Model_Ptr mdl = Model::Create("bbox", Primitives::CreateQuadRectangle(settings.bx, settings.by, true));
	mdl->EnableWireFrameMode();
	scene.Add(mdl);

	//scene.Add(TransformObject::Create("origin"));

}

void AppLayer::InitPhysics() {
	//Compute Shaders
	solver = StagedComputeShader::Create("solver", "assets/shaders/solver/solver.comp", 6);
	prefixSum = StagedComputeShader::Create("prefixSum", "assets/shaders/solver/prefix.sum.comp", 4);

	//Create particle system
	particleSystem = deprecated_ParticleSystem::Create("ParticleSystem", settings.pThread);
	Shared<Mesh> particle = Primitives::CreatePoint();
	particle->Rename("particle");
	particle->SetShader(particleShader);
	particleSystem->SetMesh(particle);
	particleSystem->SetDisplayMode(deprecated_ParticleSystemDisplayMode::POINT_SPRITE);


	Shared<Mesh> binInstance = Primitives::CreateQuadRectangle(settings.bWidth, settings.bWidth, true);
	binInstance->Rename("bin");
	binInstance->SetShader(binShader);
	binSystem = deprecated_ParticleSystem::Create("BinSystem", settings.bThread);
	binSystem->SetDisplayMode(deprecated_ParticleSystemDisplayMode::MESH);
	binSystem->SetMesh(binInstance);
	binSystem->EnableWireFrameMode();

	solver->SetWorkgroupLayout(settings.pWkgCount);
	prefixSum->SetWorkgroupLayout(settings.bWkgCount);


	//Allocate Buffers
	Console::info() << "Particle struct size :" << sizeof(Particle) << Console::endl;
	particleBuffer = SSBO<Particle>::Create("ParticleBuffer");
	particleBuffer->Allocate(settings.pThread);
	particleCpyBuffer = SSBO<Particle>::Create("ParticleCpyBuffer");
	particleCpyBuffer->Allocate(settings.pThread);

	Console::info() << "Bin struct size :" << sizeof(Bin) << Console::endl;
	binBuffer = SSBO<Bin>::Create("BinBuffer");
	binBuffer->Allocate(settings.bThread);

	sortedIndexBuffer = SSBO<GLuint>::Create("SortedIndexBuffer");
	sortedIndexBuffer->Allocate(settings.pThread);


	particleBuffer->SetBindingPoint(0);
	particleCpyBuffer->SetBindingPoint(1);
	sortedIndexBuffer->SetBindingPoint(2);
	binBuffer->SetBindingPoint(3);
	

	//Attach Buffers
	particleSystem->AddComputeShader(solver);
	particleSystem->AddStorageBuffer(particleBuffer);
	particleSystem->AddStorageBuffer(particleCpyBuffer);
	particleSystem->AddStorageBuffer(sortedIndexBuffer);
	particleSystem->AddStorageBuffer(binBuffer);


	binSystem->AddComputeShader(prefixSum);
	binSystem->AddStorageBuffer(binBuffer);
	
	scene.Add(particleSystem);
	scene.Add(binSystem);
	//scene.Add(constraintSystem);
	binSystem->Hide();
}

void AppLayer::ResetSimulation() {
	elapsedTime = 0;
	particleBuffer->FreeHostMemory();
	
	Console::info() << "Generating particles..." << Console::endl;

	float spacing = settings.particleRadius * 2.0;
	auto& cpu_particles = particleBuffer->GetDeviceBuffer();

	Particle buf;
	buf.mass = settings.particleMass;//inverse mass
	buf.density = 1.0; // phase
	buf.velocity = glm::vec2(0);
	buf.pvelocity = glm::vec2(0);
	
	buf.phase = FLUID; // phase
	glm::vec2 cubeSize = glm::vec2(50, 50);
	glm::ivec2 icubeSize = glm::vec2(cubeSize.x / spacing, cubeSize.y / spacing);

	for (int xi = 0; xi <= cubeSize.x / spacing; xi++)
		for (int yi = 0; yi <= cubeSize.y / spacing; yi++){
				float x = (xi * spacing) - (cubeSize.x / 2.0) - 45;
				float y = (yi * spacing) - (cubeSize.y / 2.0);
				buf.id = cpu_particles.size();
				buf.position.x = x;
				buf.position.y = y;
				buf.pposition = buf.position;
				buf.binIndex = cpu_particles.size();
				cpu_particles.push_back(buf);
			}
	
	numParticles = cpu_particles.size();
	particleSystem->SetInstancesCount(settings.pThread);
	settings.pThread = numParticles;
	UpdateBufferSettings();

	particleBuffer->Bind();
	particleBuffer->Clear();
	particleBuffer->Upload();
	
	binBuffer->Bind();
	binBuffer->Clear();
	
	auto& cpu_sortedIndexBuffer = sortedIndexBuffer->GetDeviceBuffer();
	for (int i = 0; i < settings.pThread; i++) cpu_sortedIndexBuffer[i] = i;
	sortedIndexBuffer->Bind();
	sortedIndexBuffer->Upload();


	particleShader->Use();
	particleShader->SetUInt("numParticles", numParticles);
	particleShader->SetFloat("smoothingRadius", settings.H); // Kernel radius // 5mm
	particleShader->SetFloat("particleRadius", settings.particleRadius); // visual radius // 5mm
	particleShader->SetFloat("particleMass", settings.particleMass); // Kernel radius // 5mm
	particleShader->SetFloat("REST_DENSITY", settings.REST_DENSITY); // Kernel radius // 5mm

	solver->Use();
	solver->SetUInt("numParticles", numParticles);
	solver->SetFloat("smoothingRadius", settings.H); // Kernel radius // 5mm
	solver->SetFloat("particleRadius", settings.particleRadius); // visual radius // 5mm
	solver->SetFloat("particleMass", settings.particleMass); // Kernel radius // 5mm
	solver->SetFloat("REST_DENSITY", settings.REST_DENSITY); // Kernel radius // 5mm

	//solver->SetInt("stabilizationPass", true);
	//Simulate(0.016);
	//solver->SetInt("stabilizationPass", false);
}


void AppLayer::NeigborSearch() {
	solver->Use();
	binBuffer->Bind();
	binBuffer->Clear(); //Reset neighbor search data
	solver->Execute(0); //Place particles in bins

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

	solver->Use();
	solver->Execute(1); //Sort
}


void AppLayer::Simulate(Merlin::Timestep ts) {

	solver->Use();
	solver->SetUInt("numParticles", numParticles); //Spawn particle after prediction
	solver->SetFloat("dt", settings.timeStep / float(settings.solver_substep)); //Spawn particle after prediction

	if (!paused) {
		elapsedTime += settings.timeStep;

		GPU_PROFILE(nns_time,
			NeigborSearch();
		)
		GPU_PROFILE(solver_substep_time,
			for (int i = 0; i < settings.solver_substep; i++) {
				solver->Execute(2); //Predict position

				for (int j = 0; j < settings.solver_iteration; j++) {
					solver->Execute(3); //Solve floor collision constraint
					solver->Execute(4); //Solve collision constraint
				}
				//solver->Execute(5); //Solve distance constraint
			})
	}
}

void AppLayer::updateFPS(Timestep ts) {
	if (FPS_sample == 0) {
		FPS = ts;
	}
	else {
		FPS += ts;
	}
	FPS_sample++;
}



void AppLayer::OnImGuiRender() {
	//ImGui::ShowDemoWindow();
	ImGui::DockSpaceOverViewport((ImGuiViewport*)0, ImGuiDockNodeFlags_PassthruCentralNode);
	ImGui::Begin("Infos");


	ImGui::LabelText(std::to_string(numParticles).c_str(), "particles");
	ImGui::LabelText(std::to_string(settings.bThread).c_str(), "bins");
	ImGui::LabelText(std::to_string(elapsedTime).c_str(), "s");

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


	static bool transparency = true;
	if (ImGui::Checkbox("Particle transparency", &transparency)) {
		if (transparency) particleSystem->SetDisplayMode(deprecated_ParticleSystemDisplayMode::POINT_SPRITE_SHADED);
		else particleSystem->SetDisplayMode(deprecated_ParticleSystemDisplayMode::POINT_SPRITE);
	}

	static bool showbed = true;
	if (ImGui::Checkbox("Show build plate", &showbed)) {
		auto bedRef = scene.GetChild("bed");
		if (bedRef) {
			if (showbed) scene.GetChild("bed")->Show();
			else scene.GetChild("bed")->Hide();
		}
	}

	ImGui::Checkbox("Integrate", &integrate);

	if (ImGui::SmallButton("Reset simulation")) {
		ResetSimulation();
		//count = 1;
	}

	static bool Pstate = true;
	if (ImGui::Checkbox("Show Particles", &Pstate)) {
		if (Pstate) particleSystem->Show();
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

	/*
	ImGui::DragInt("Solver iteration", &settings.solver_iteration, 1, 1, 50);
	*/

	ImGui::DragInt("Solver substep", &settings.solver_substep, 1, 1, 200);
	ImGui::DragInt("Solver iteratino", &settings.solver_iteration, 1, 1, 200);

	if (ImGui::DragFloat3("Camera position", &model_matrix_translation.x, -100.0f, 100.0f)) {
		camera->SetPosition(model_matrix_translation);
	}

	if (ImGui::SliderFloat("Simulation speed", &sim_speed, 0.0, 5.0f)) {
		solver->Use();
		solver->SetFloat("speed", sim_speed);
	}

	if (ImGui::SliderFloat("Smoothing radius", &settings.H, 0.3, 1.2 * settings.bWidth)) {
		solver->Use();
		solver->SetFloat("smoothingRadius", settings.H); // Kernel radius // 5mm
		particleShader->Use();
		particleShader->SetFloat("smoothingRadius", settings.H); // Kernel radius // 5mm
	}
	if (ImGui::SliderFloat("particle radius", &settings.particleRadius, 0.0, 2.0)) {
		solver->Use();
		solver->SetFloat("particleRadius", settings.particleRadius);
		particleShader->Use();
		particleShader->SetFloat("particleRadius", settings.particleRadius); // Kernel radius // 5mm
	}

	if (ImGui::SliderFloat("Rest density", &settings.REST_DENSITY, 0.0, 2.0)) {
		solver->Use();
		solver->SetFloat("REST_DENSITY", settings.REST_DENSITY); // Kernel radius // 5mm
		particleShader->Use();
		particleShader->SetFloat("REST_DENSITY", settings.REST_DENSITY); // Kernel radius // 5mm
	}

	static float pressureM = 0.5;
	if (ImGui::SliderFloat("Pressure multiplier", &pressureM, 0.0, 1.0)) {
		solver->Use();
		solver->SetFloat("pressureMultiplier", pressureM * 0.001); // Kernel radius // 5mm
	}

	static float stiffness = 5000;
	if (ImGui::SliderFloat("stiffness", &stiffness, 0.0, 2000000)) {
		solver->Use();
		solver->SetFloat("stiffness", stiffness); // Kernel radius // 5mm
	}

	static float visco = 0.5;
	if (ImGui::SliderFloat("Viscosity", &visco, 0.0, 1.0)) {
		solver->Use();
		solver->SetFloat("alphaVisco", visco * 0.1); // Kernel radius // 5mm
	}

	if (ImGui::SliderFloat("Fluid particle mass", &settings.particleMass, 0.1, 2.0)) {
		solver->Use();
		solver->SetFloat("particleMass", settings.particleMass);
	}

	static int colorMode = 0;
	static const char* options[] = { "Solid color", "Bin index", "Density", "Temperature", "Velocity", "Mass", "Neighbors" };
	if (ImGui::ListBox("Colored field", &colorMode, options, 7)) {
		particleShader->Use();
		particleShader->SetInt("colorCycle", colorMode);
		binShader->Use();
		binShader->SetInt("colorCycle", colorMode);
	}

	static int particleTest = 50;
	static int binTest = 1459;
	if (colorMode == 6) {
		if (ImGui::DragInt("Particle to test", &particleTest)) {
			particleShader->Use();
			particleShader->SetUInt("particleTest", particleTest);
			binShader->Use();
			binShader->SetUInt("particleTest", particleTest);
		}

		ImGui::LabelText("Bin selector", "");

		bool changed = false;

		ImGui::LabelText("Current Bin", std::to_string(binTest).c_str());

		if (ImGui::SmallButton("X+")) { binTest++; changed = true; }
		if (ImGui::SmallButton("X-")) { binTest--; changed = true; }
		if (ImGui::SmallButton("Y+")) { binTest += (settings.bx / settings.bWidth); changed = true; }
		if (ImGui::SmallButton("Y-")) { binTest -= (settings.bx / settings.bWidth); changed = true; }
		if (ImGui::SmallButton("Z+")) { binTest += int(settings.by / settings.bWidth) * int(settings.bx / settings.bWidth); changed = true; }
		if (ImGui::SmallButton("Z-")) { binTest -= int(settings.by / settings.bWidth) * int(settings.bx / settings.bWidth); changed = true; }

		if (changed) {
			binShader->Use();
			binShader->SetUInt("binTest", binTest);
		}
	}

	ImGui::End();

	ImGui::Begin("Performance");
	ImGui::Text("Nearest Neighbor Search %.1f ms", nns_time);
	ImGui::Text("Jacobi solver %.1f ms", solver_substep_time);
	ImGui::Text("Physics %.1f ms", solver_total_time);
	ImGui::Text("Render %.1f ms", render_time);
	ImGui::Text("Total frame time %.1f ms", total_time);
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
		sortedIndexBuffer->Bind();
		sortedIndexBuffer->Download();

		/*
		std::vector<FluidParticle> sorted;
		sorted.resize(particleBuffer->GetDeviceBuffer().size());

		std::vector<FluidParticle> bugged;
		for (int i = 0; i < numParticles; i++) {
			sorted[particleBuffer->GetDeviceBuffer()[i].newIndex] = particleBuffer->GetDeviceBuffer()[i];
		}*/

		throw("DEBUG");
		Console::info() << "DEBUG" << Console::endl;
	}
	ImGui::End();
}
