#include "AppLayer.h"

#include <GLFW/glfw3.h>
#include <iostream>
#include <iomanip>

using namespace Merlin;


#define PROFILE(VAR, CODE) double start_ ## VAR ## _time = glfwGetTime(); CODE VAR = (glfwGetTime() - start_ ## VAR ## _time)*1000.0;
#define GPU_PROFILE(VAR, CODE) double start_ ## VAR ## _time = glfwGetTime(); CODE glFinish(); VAR = (glfwGetTime() - start_ ## VAR ## _time)*1000.0;

#define PROFILE_BEGIN(STARTVAR) STARTVAR = glfwGetTime();
#define PROFILE_END(STARTVAR, VAR) VAR = (glfwGetTime() - STARTVAR)*1000.0

AppLayer::AppLayer() {
	Window* w = &Application::Get().GetWindow();
	int height = w->GetHeight();
	int width = w->GetWidth();
	camera = CreateShared<Camera>(width, height, Projection::Perspective);
	camera->setNearPlane(0.1f);
	camera->setFarPlane(1800.0f);
	camera->setFOV(60); //Use 90.0f as we are using cubemaps
	camera->SetPosition(glm::vec3(0.0f, -300.0f, 80));
	camera->SetRotation(glm::vec3(0, 10, -270));
	cameraController = CreateShared<CameraController3D>(camera);
	cameraController->SetZoomLevel(1);
	cameraController->SetCameraSpeed(100);
}

AppLayer::~AppLayer() {}

void AppLayer::OnAttach() {
	EnableGLDebugging();
	//ImGui::LoadIniSettingsFromDisk("imgui.ini");
	Console::SetLevel(ConsoleLevel::_TRACE);
	glfwSwapInterval(0);

	InitGraphics();
	InitPhysics();

	particleShader->Use();
	particleShader->Attach(*positionBuffer);
	particleShader->Attach(*predictedPositionBuffer);
	particleShader->Attach(*velocityBuffer);
	particleShader->Attach(*densityBuffer);
	particleShader->Attach(*lambdaBuffer);
	particleShader->Attach(*metaBuffer);

	binShader->Use();
	binShader->Attach(*positionBuffer);
	binShader->Attach(*predictedPositionBuffer);
	binShader->Attach(*velocityBuffer);
	binShader->Attach(*densityBuffer);
	binShader->Attach(*lambdaBuffer);
	binShader->Attach(*metaBuffer);
	binShader->Attach(*binBuffer);

	ResetSimulation();
}

void AppLayer::OnDetach() {}

void AppLayer::OnEvent(Event& event) {
	camera->OnEvent(event);
	cameraController->OnEvent(event);
}

float t = 0.0;

void AppLayer::OnUpdate(Timestep ts) {
	cameraController->OnUpdate(ts);
	PROFILE_END(total_start_time, total_time);
	PROFILE_BEGIN(total_start_time);

	updateFPS(ts);

	GPU_PROFILE(render_time,
		renderer.Clear();
	renderer.RenderScene(scene, *camera);
	)

	if (!paused) {
		GPU_PROFILE(solver_total_time,
			Simulate(0.016);
		)
	}
}



void AppLayer::SyncUniforms() {

	solver->Use();

	//settings.timestep.Sync(*solver);
	settings.particleMass.Sync(*solver);
	settings.restDensity.Sync(*solver);
	solver->SetUInt("numParticles", numParticles);
	solver->SetFloat("dt", settings.timestep.value() / float(settings.solver_substep)); //Spawn particle after prediction
	solver->SetFloat("artificialViscosityMultiplier", settings.artificialViscosityMultiplier.value() * 0.01);
	solver->SetFloat("artificialPressureMultiplier", settings.artificialPressureMultiplier.value() * 0.01);

	particleShader->Use();
	particleShader->SetUInt("numParticles", numParticles);
	settings.restDensity.Sync(*particleShader);

	prefixSum->Use();
	prefixSum->SetUInt("dataSize", settings.bThread); //data size
	prefixSum->SetUInt("blockSize", settings.blockSize); //block size
}



void AppLayer::ApplyBufferSettings() {

	settings.pWkgCount = (settings.pThread + settings.pWkgSize - 1) / settings.pWkgSize; //Total number of workgroup needed
	settings.blockSize = floor(log2f(settings.bThread));
	settings.blocks = (settings.bThread + settings.blockSize - 1) / settings.blockSize;
	settings.bWkgCount = (settings.blocks + settings.bWkgSize - 1) / settings.bWkgSize; //Total number of workgroup needed

	solver->SetWorkgroupLayout(settings.pWkgCount);
	prefixSum->SetWorkgroupLayout(settings.bWkgCount);
	particleSystem->SetInstancesCount(settings.pThread);
	binSystem->SetInstancesCount(settings.bThread);

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
	particleShader->SetVec3("lightPos", glm::vec3(0, 0, 500));

	binShader = Shader::Create("bins", "assets/shaders/bin.vert", "assets/shaders/bin.frag");
	binShader->noTexture();
	binShader->noMaterial();

	modelShader = Shader::Create("model", "assets/common/shaders/default.model.vert", "assets/common/shaders/default.model.frag");
	modelShader->SetVec3("lightPos", glm::vec3(0, 0, 50));
	//modelShader->noTexture();

	particleShader->Use();
	particleShader->SetInt("colorCycle", 4);
	binShader->Use();
	binShader->SetInt("colorCycle", 4);

	renderer.AddShader(particleShader);
	renderer.AddShader(binShader);
	renderer.AddShader(modelShader);

	Shared<Shader> skyShader = Shader::Create("skybox", "assets/common/shaders/default.skybox.vert", "assets/common/shaders/default.skybox.frag");
	Shared<SkyBox> sky = SkyBox::Create("Sky");
	sky->SetShader(skyShader);
	scene.Add(sky);

	Shared<Model> floor = ModelLoader::LoadModel("./assets/models/bed.stl");
	floor->Translate(glm::vec3(0.75, -0.25, -0.1));
	floor->Scale(glm::vec3(1.025, 1.025, 1.0));
	floor->SetMaterial("chrome");
	floor->SetShader("model");
	scene.Add(floor);

	//modelShader->Use();
	//modelShader->SetVec3("lightPos", glm::vec3(0.0, 10.0, 10));

	Shared<Model> floorSurface = Model::Create("floorSurface", Primitives::CreateRectangle(316, 216));
	floorSurface->Translate(glm::vec3(0.75, -0.25, 0));

	Shared<Material> floorMat2 = CreateShared<Material>("floorMat2");
	floorMat2->SetAmbient(glm::vec3(0.02));
	floorMat2->SetDiffuse(glm::vec3(0.95));
	floorMat2->SetSpecular(glm::vec3(0.99));
	floorMat2->SetShininess(0.99);
	floorMat2->LoadTexture("assets/textures/bed.png", TextureType::COLOR);


	floorSurface->SetMaterial(floorMat2);
	floorSurface->SetShader("model");
	scene.Add(floorSurface);

	Model_Ptr bbox = Model::Create("bbox", Primitives::CreateQuadCube(settings.bb.x, settings.bb.y, settings.bb.z));
	bbox->EnableWireFrameMode();
	bbox->SetMaterial("default");
	bbox->Translate(glm::vec3(0, 0, settings.bb.z / 2.0));
	scene.Add(bbox);

	scene.Add(TransformObject::Create("origin"));

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
	particleSystem->SetDisplayMode(deprecated_ParticleSystemDisplayMode::POINT_SPRITE_SHADED);

	Shared<Mesh> binInstance = Primitives::CreateQuadCube(settings.bWidth, false);
	binInstance->Rename("bin");
	binInstance->SetShader(binShader);
	binSystem = deprecated_ParticleSystem::Create("BinSystem", settings.bThread);
	binSystem->SetDisplayMode(deprecated_ParticleSystemDisplayMode::MESH);
	binSystem->SetMesh(binInstance);
	binSystem->EnableWireFrameMode();

	solver->SetWorkgroupLayout(settings.pWkgCount);
	prefixSum->SetWorkgroupLayout(settings.bWkgCount);

	// Allocate Buffer and double buffering
	positionBuffer = SSBO<glm::vec4>::Create("PositionBuffer", settings.pThread);
	cpyPositionBuffer = SSBO<glm::vec4>::Create("cpyPositionBuffer", settings.pThread);
	predictedPositionBuffer = SSBO<glm::vec4>::Create("PredictedPositionBuffer", settings.pThread);
	cpyPredictedPositionBuffer = SSBO<glm::vec4>::Create("cpyPredictedPositionBuffer", settings.pThread);
	velocityBuffer = SSBO<glm::vec4>::Create("VelocityBuffer", settings.pThread);
	cpyVelocityBuffer = SSBO<glm::vec4>::Create("cpyVelocityBuffer", settings.pThread);
	densityBuffer = SSBO<float>::Create("DensityBuffer", settings.pThread);
	cpyDensityBuffer = SSBO<float>::Create("cpyDensityBuffer", settings.pThread);
	lambdaBuffer = SSBO<float>::Create("LambdaBuffer", settings.pThread);
	cpyLambdaBuffer = SSBO<float>::Create("cpyLambdaBuffer", settings.pThread);
	metaBuffer = SSBO<glm::uvec4>::Create("MetaBuffer", settings.pThread);
	cpymetaBuffer = SSBO<glm::uvec4>::Create("cpyMetaBuffer", settings.pThread);

	Console::info() << "Bin struct size :" << sizeof(Bin) << Console::endl;
	binBuffer = SSBO<Bin>::Create("BinBuffer", settings.bThread);

	// Set binding points for position and its copy
	positionBuffer->setBindingPoint(0);
	cpyPositionBuffer->setBindingPoint(1);
	predictedPositionBuffer->setBindingPoint(2);
	cpyPredictedPositionBuffer->setBindingPoint(3);
	velocityBuffer->setBindingPoint(4);
	cpyVelocityBuffer->setBindingPoint(5);
	densityBuffer->setBindingPoint(6);
	cpyDensityBuffer->setBindingPoint(7);
	lambdaBuffer->setBindingPoint(8);
	cpyLambdaBuffer->setBindingPoint(9);
	metaBuffer->setBindingPoint(10);
	cpymetaBuffer->setBindingPoint(11);
	binBuffer->setBindingPoint(13);

	//Attach Buffers
	particleSystem->AddComputeShader(solver);
	particleSystem->AddStorageBuffer(positionBuffer);
	particleSystem->AddStorageBuffer(cpyPositionBuffer);
	particleSystem->AddStorageBuffer(predictedPositionBuffer);
	particleSystem->AddStorageBuffer(cpyPredictedPositionBuffer);
	particleSystem->AddStorageBuffer(velocityBuffer);
	particleSystem->AddStorageBuffer(cpyVelocityBuffer);
	particleSystem->AddStorageBuffer(densityBuffer);
	particleSystem->AddStorageBuffer(cpyDensityBuffer);
	particleSystem->AddStorageBuffer(lambdaBuffer);
	particleSystem->AddStorageBuffer(cpyLambdaBuffer);
	particleSystem->AddStorageBuffer(metaBuffer);
	particleSystem->AddStorageBuffer(cpymetaBuffer);
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
	Console::info() << "Generating particles..." << Console::endl;

	float spacing = settings.particleRadius * 2.0;
	auto cpu_position = positionBuffer->getEmptyArray();
	auto cpu_predictedPosition = predictedPositionBuffer->getEmptyArray();
	auto cpu_velocity = velocityBuffer->getEmptyArray();
	auto cpu_density = densityBuffer->getEmptyArray();
	auto cpu_lambda = lambdaBuffer->getEmptyArray();
	auto cpu_meta = metaBuffer->getEmptyArray();


	glm::vec3 cubeSize = glm::vec3(60, 195, 50);
	glm::ivec3 icubeSize = glm::vec3(cubeSize.x / spacing, cubeSize.y / spacing, cubeSize.z / spacing);

	numParticles = 0;
	for (int xi = 0; xi <= cubeSize.x / spacing; xi++) {
		for (int yi = 0; yi <= cubeSize.y / spacing; yi++) {
			for (int zi = 0; zi <= cubeSize.z / spacing; zi++) {
				float x = ((xi + 1) * spacing) - (settings.bb.x / 2.0);
				float y = ((yi + 1) * spacing) - (settings.bb.y / 2.0);
				float z = ((zi + 1) * spacing);

				cpu_position.push_back(glm::vec4(x, y, z, 0.0));
				cpu_predictedPosition.push_back(glm::vec4(x, y, z, 0.0));
				cpu_velocity.push_back(glm::vec4(0));
				cpu_density.push_back(0.0);
				cpu_lambda.push_back(0.0);
				cpu_meta.push_back(glm::uvec4(FLUID, numParticles, numParticles, 0.0));
				numParticles++;
			}
		}
	}

	for (int xi = 0; xi <= cubeSize.x / spacing; xi++) {
		for (int yi = 0; yi <= cubeSize.y / spacing; yi++) {
			for (int zi = 0; zi <= cubeSize.z / spacing; zi++) {
				float x = -((xi + 1) * spacing) + (settings.bb.x / 2.0);
				float y = ((yi + 1) * spacing) - (settings.bb.y / 2.0);
				float z = ((zi + 1) * spacing);

				cpu_position.push_back(glm::vec4(x, y, z, 0.0));
				cpu_predictedPosition.push_back(glm::vec4(x, y, z, 0.0));
				cpu_velocity.push_back(glm::vec4(0));
				cpu_density.push_back(0.0);
				cpu_lambda.push_back(0.0);
				cpu_meta.push_back(glm::uvec4(FLUID, numParticles, numParticles, 0.0));
				numParticles++;
			}
		}
	}

	Console::info() << "Uploading buffer on device..." << Console::endl;

	settings.pThread = numParticles;
	particleSystem->SetInstancesCount(settings.pThread);

	ApplyBufferSettings();
	SyncUniforms();

	positionBuffer->write(cpu_position);
	predictedPositionBuffer->write(cpu_predictedPosition);
	velocityBuffer->write(cpu_velocity);
	densityBuffer->write(cpu_density);
	lambdaBuffer->write(cpu_lambda);
	metaBuffer->write(cpu_meta);

}


void AppLayer::NeigborSearch() {
	prefixSum->Use();
	prefixSum->SetUInt("dataSize", settings.bThread); //data size
	prefixSum->SetUInt("blockSize", settings.blockSize); //block size

	prefixSum->Use();
	prefixSum->Execute(4);// clear bins

	solver->Use();
	solver->Execute(0); //Place particles in bins

	prefixSum->Use();
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

	GPU_PROFILE(solver_substep_time,
		for (int i = 0; i < settings.solver_substep; i++) {
			solver->Execute(2);
			GPU_PROFILE(nns_time,
				NeigborSearch();
			)

				if (integrate) {
					GPU_PROFILE(jacobi_time,
						for (int j = 0; j < settings.solver_iteration; j++) {
							solver->Execute(3);
							solver->Execute(4);
						}
					)
						solver->Execute(5);

				}
		}
	)
		elapsedTime += settings.timestep.value();

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
		}
	}


	static bool transparency = true;
	if (ImGui::Checkbox("Particle shading", &transparency)) {
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

	static bool Whirlpool = false;
	if (ImGui::Checkbox("whirlpool", &Whirlpool)) {
		solver->Use();
		solver->SetInt("whirlpool", Whirlpool);
	}

	ImGui::DragInt("Solver substep", &settings.solver_substep, 1, 1, 200);
	ImGui::DragInt("Solver iteration", &settings.solver_iteration, 1, 1, 200);

	if (ImGui::DragFloat3("Camera position", &model_matrix_translation.x, -100.0f, 100.0f)) {
		camera->SetPosition(model_matrix_translation);
	}

	if (ImGui::InputFloat("Time step", &settings.timestep.value(), 0.0, 0.02f)) {
		solver->Use();
		solver->SetFloat("dt", settings.timestep.value());
	}



	if (ImGui::SliderFloat("Fluid particle mass", &settings.particleMass.value(), 0.1, 2.0)) {
		solver->Use();
		settings.particleMass.Sync(*solver);
	}
	if (ImGui::SliderFloat("Rest density", &settings.restDensity.value(), 0.0, 2.0)) {
		solver->Use();
		settings.restDensity.Sync(*solver);
		particleShader->Use();
		settings.restDensity.Sync(*particleShader);
	}
	if (ImGui::SliderFloat("Pressure multiplier", &settings.artificialPressureMultiplier.value(), 0.0, 10.0)) {
		solver->Use();
		solver->SetFloat("artificialPressureMultiplier", settings.artificialPressureMultiplier.value() * 0.001);
	}
	if (ImGui::SliderFloat("Viscosity", &settings.artificialViscosityMultiplier.value(), 0.0, 1000.0)) {
		solver->Use();
		solver->SetFloat("artificialViscosityMultiplier", settings.artificialViscosityMultiplier.value()*0.001);
	}

	static float whirlpool = 50;
	if (ImGui::SliderFloat("Whirlpool", &whirlpool, 0.0, 1000.0)) {
		solver->Use();
		solver->SetFloat("whirlpoolIntensity", whirlpool);
	}

	static int colorMode = 4;
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
		if (ImGui::SmallButton("Y+")) { binTest += (settings.bb.x / settings.bWidth); changed = true; }
		if (ImGui::SmallButton("Y-")) { binTest -= (settings.bb.x / settings.bWidth); changed = true; }
		if (ImGui::SmallButton("Z+")) { binTest += int(settings.bb.y / settings.bWidth) * int(settings.bb.x / settings.bWidth); changed = true; }
		if (ImGui::SmallButton("Z-")) { binTest -= int(settings.bb.y / settings.bWidth) * int(settings.bb.x / settings.bWidth); changed = true; }

		if (changed) {
			binShader->Use();
			binShader->SetUInt("binTest", binTest);
		}
	}

	ImGui::End();

	ImGui::Begin("Performance");
	ImGui::Text("Nearest Neighbor Search %.1f ms", nns_time);
	ImGui::Text("Substep solver %.1f ms", solver_substep_time - nns_time);
	ImGui::Text("Jacobi iteration %.1f ms", jacobi_time);
	ImGui::Text("Total physics time %.1f ms", solver_total_time);
	ImGui::Text("Render time %.1f ms", render_time);
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
		GPU_PROFILE(solver_total_time,
			Simulate(0.016);
		)
	}

	if (ImGui::Button("Debug")) {
		auto buf = positionBuffer->read();
		auto buf0 = metaBuffer->read();
		auto buf1 = binBuffer->read();
		throw("DEBUG");
		Console::info() << "DEBUG" << Console::endl;
	}
	ImGui::End();
}
