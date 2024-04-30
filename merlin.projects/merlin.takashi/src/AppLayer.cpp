#include "Applayer.h"

#include <GLFW/glfw3.h>
#include <iostream>
#include <iomanip>

using namespace Merlin;


#define PROFILE(VAR, CODE) double start_ ## VAR ## _time = glfwGetTime(); CODE VAR = (glfwGetTime() - start_ ## VAR ## _time)*1000.0;
#define GPU_PROFILE(VAR, CODE) double start_ ## VAR ## _time = glfwGetTime(); CODE glFinish(); VAR = (glfwGetTime() - start_ ## VAR ## _time)*1000.0;

#define PROFILE_BEGIN(STARTVAR) STARTVAR = glfwGetTime();
#define PROFILE_END(STARTVAR, VAR) VAR = (glfwGetTime() - STARTVAR)*1000.0

AppLayer::AppLayer() {
	Window* w = &Application::get().getWindow();
	int height = w->getHeight();
	int width = w->getWidth();
	camera = createShared<Camera>(width, height, Projection::Perspective);
	camera->setNearPlane(0.1f);
	camera->setFarPlane(1800.0f);
	camera->setFOV(60); //Use 90.0f as we are using cubemaps
	camera->setPosition(glm::vec3(0.0f, -300.0f, 80));
	camera->setRotation(glm::vec3(0, 10, -270));
	cameraController = createShared<CameraController3D>(camera);
	cameraController->setZoomLevel(1);
	cameraController->setCameraSpeed(100);
}

AppLayer::~AppLayer() {}

void AppLayer::onAttach() {
	enableGLDebugging();
	//ImGui::LoadIniSettingsFromDisk("imgui.ini");
	Console::setLevel(ConsoleLevel::_TRACE);
	glfwSwapInterval(0);

	bins = ParticleSystem::Create("bins", settings.pThread);

	particles = ParticleSystem::Create("particles", settings.pThread);

	InitGraphics();
	InitPhysics();

	particleShader->use();
	particleShader->attach(*particles->getFieldBuffer("positionBuffer"));
	particleShader->attach(*particles->getFieldBuffer("predictedPositionBuffer"));
	particleShader->attach(*particles->getFieldBuffer("velocityBuffer"));
	particleShader->attach(*particles->getFieldBuffer("densityBuffer"));
	particleShader->attach(*particles->getFieldBuffer("lambdaBuffer"));
	particleShader->attach(*particles->getFieldBuffer("metaBuffer"));

	binShader->use();
	binShader->attach(*particles->getFieldBuffer("positionBuffer"));
	binShader->attach(*particles->getFieldBuffer("predictedPositionBuffer"));
	binShader->attach(*particles->getFieldBuffer("velocityBuffer"));
	binShader->attach(*particles->getFieldBuffer("densityBuffer"));
	binShader->attach(*particles->getFieldBuffer("lambdaBuffer"));
	binShader->attach(*particles->getFieldBuffer("metaBuffer"));
	binShader->attach(*particles->getFieldBuffer("binBuffer"));

	ResetSimulation();
}

void AppLayer::onDetach() {}

void AppLayer::onEvent(Event& event) {
	camera->onEvent(event);
	cameraController->onEvent(event);
}

float t = 0.0;

void AppLayer::onUpdate(Timestep ts) {
	cameraController->onUpdate(ts);
	PROFILE_END(total_start_time, total_time);
	PROFILE_BEGIN(total_start_time);

	updateFPS(ts);

	GPU_PROFILE(render_time,
		renderer.clear();
	renderer.renderScene(scene, *camera);
	)

	if (!paused) {
		GPU_PROFILE(solver_total_time,
			Simulate(0.016);
		)
	}
}



void AppLayer::SyncUniforms() {

	solver->use();

	//settings.timestep.sync(*solver);
	settings.particleMass.sync(*solver);
	settings.restDensity.sync(*solver);
	solver->setUInt("numParticles", numParticles);
	solver->setFloat("dt", settings.timestep.value() / float(settings.solver_substep)); //Spawn particle after prediction
	solver->setFloat("artificialViscosityMultiplier", settings.artificialViscosityMultiplier.value() * 0.01);
	solver->setFloat("artificialPressureMultiplier", settings.artificialPressureMultiplier.value() * 0.01);

	particleShader->use();
	particleShader->setUInt("numParticles", numParticles);
	settings.restDensity.sync(*particleShader);

	prefixSum->use();
	prefixSum->setUInt("dataSize", settings.bThread); //data size
	prefixSum->setUInt("blockSize", settings.blockSize); //block size
}



void AppLayer::ApplyBufferSettings() {

	settings.pWkgCount = (settings.pThread + settings.pWkgSize - 1) / settings.pWkgSize; //Total number of workgroup needed
	settings.blockSize = floor(log2f(settings.bThread));
	settings.blocks = (settings.bThread + settings.blockSize - 1) / settings.blockSize;
	settings.bWkgCount = (settings.blocks + settings.bWkgSize - 1) / settings.bWkgSize; //Total number of workgroup needed

	solver->SetWorkgroupLayout(settings.pWkgCount);
	prefixSum->SetWorkgroupLayout(settings.bWkgCount);
	particles->setInstancesCount(settings.pThread);
	bins->setInstancesCount(settings.bThread);




}

void AppLayer::InitGraphics() {
	// init OpenGL stuff
	renderer.initialize();
	renderer.setBackgroundColor(0.203, 0.203, 0.203, 1.0);
	renderer.enableTransparency();
	renderer.enableSampleShading();

	particleShader = Shader::create("particle", "assets/shaders/particle.vert", "assets/shaders/particle.frag");
	particleShader->noTexture();
	particleShader->noMaterial();
	particleShader->setVec3("lightPos", glm::vec3(0, 0, 500));
	particleShader->setInt("colorCycle", 4);

	binShader = Shader::create("bins", "assets/shaders/bin.vert", "assets/shaders/bin.frag");
	binShader->noTexture();
	binShader->noMaterial();
	binShader->setInt("colorCycle", 4);

	renderer.addShader(particleShader);
	renderer.addShader(binShader);
	renderer.addShader(modelShader);

	Shared<Mesh> particle = Primitives::createPoint();
	particle->rename("particle");
	particle->setShader(particleShader);
	particles->setMesh(particle);
	particles->setDisplayMode(ParticleSystemDisplayMode::POINT_SPRITE_SHADED);

	Shared<Mesh> binInstance = Primitives::createQuadCube(settings.bWidth, false);
	binInstance->rename("bin");
	binInstance->setShader(binShader);
	particles->setDisplayMode(ParticleSystemDisplayMode::MESH);
	bins->setMesh(binInstance);
	bins->enableWireFrameMode();


	modelShader = Shader::create("model", "assets/common/shaders/default.model.vert", "assets/common/shaders/default.model.frag");
	modelShader->setVec3("lightPos", glm::vec3(0, 0, 50));
	//modelShader->noTexture();

	Shared<Shader> skyShader = Shader::create("skybox", "assets/common/shaders/default.skybox.vert", "assets/common/shaders/default.skybox.frag");
	Shared<SkyBox> sky = SkyBox::create("Sky");
	sky->setShader(skyShader);
	scene.add(sky);

	Shared<Model> floor = ModelLoader::loadModel("./assets/models/bed.stl");
	floor->translate(glm::vec3(0.75, -0.25, -0.1));
	floor->scale(glm::vec3(1.025, 1.025, 1.0));
	floor->setMaterial("chrome");
	floor->setShader("model");
	scene.add(floor);

	Model_Ptr bbox = Model::create("bbox", Primitives::createQuadCube(settings.bb.x, settings.bb.y, settings.bb.z));
	bbox->enableWireFrameMode();
	bbox->setMaterial("default");
	bbox->translate(glm::vec3(0, 0, settings.bb.z / 2.0));
	scene.add(bbox);
	scene.add(TransformObject::create("origin"));

}

void AppLayer::InitPhysics() {
	//Compute Shaders
	solver = StagedComputeShader::create("solver", "assets/shaders/solver/solver.comp", 6);
	prefixSum = StagedComputeShader::create("prefixSum", "assets/shaders/solver/prefix.sum.comp", 4);

	//create particle system

	solver->SetWorkgroupLayout(settings.pWkgCount);
	prefixSum->SetWorkgroupLayout(settings.bWkgCount);

	bins->addShader(prefixSum);
	bins->addField<Bin>("BinBuffer");

	particles->addShader("solver", solver);
	particles->addField<glm::vec4>("PositionBuffer");
	particles->addField<glm::vec4>("cpyPositionBuffer");
	particles->addField<glm::vec4>("PredictedPositionBuffer");
	particles->addField<glm::vec4>("cpyPredictedPositionBuffer");
	particles->addField<glm::vec4>("VelocityBuffer");
	particles->addField<glm::vec4>("cpyVelocityBuffer");
	particles->addField<float>("DensityBuffer");
	particles->addField<float>("cpyDensityBuffer");
	particles->addField<float>("LambdaBuffer");
	particles->addField<float>("cpyLambdaBuffer");
	particles->addField<glm::uvec4>("MetaBuffer");
	particles->addField<glm::uvec4>("cpyMetaBuffer");
	particles->addField<Bin>(bins->getField<Bin>("bins"));

	scene.add(particles);
	scene.add(bins);
	//scene.Add(constraintSystem);
	bins->hide();
}

void AppLayer::ResetSimulation() {
	elapsedTime = 0;
	Console::info() << "Generating particles..." << Console::endl;

	float spacing = settings.particleRadius * 2.0;
	auto cpu_position = std::vector<glm::vec4>();
	auto cpu_predictedPosition = std::vector<glm::vec4>();
	auto cpu_velocity = std::vector<glm::vec4>();
	auto cpu_density = std::vector<float>();
	auto cpu_lambda = std::vector<float>();
	auto cpu_meta = std::vector<glm::uvec4>();

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
	particles->setInstancesCount(settings.pThread);

	ApplyBufferSettings();
	SyncUniforms();

	particles->writeField<glm::vec4>("positionBuffer", cpu_position);
	particles->writeField<glm::vec4>("predictedPositionBuffer", cpu_predictedPosition);
	particles->writeField<glm::vec4>("velocityBuffer", cpu_velocity);
	particles->writeField<float>("densityBuffer", cpu_density);
	particles->writeField<float>("lambdaBuffer", cpu_lambda);
	particles->writeField<glm::uvec4>("metaBuffer", cpu_meta);
}


void AppLayer::NeigborSearch() {
	prefixSum->use();
	prefixSum->setUInt("dataSize", settings.bThread); //data size
	prefixSum->setUInt("blockSize", settings.blockSize); //block size

	prefixSum->use();
	prefixSum->execute(4);// clear bins

	solver->use();
	solver->execute(0); //Place particles in bins

	prefixSum->use();
	prefixSum->execute(0);// local prefix sum

	//Binary tree on rightmost element of blocks
	GLuint steps = settings.blockSize;
	UniformObject<GLuint> space("space");
	space.value = 1;

	for (GLuint step = 0; step < steps; step++) {
		// Calls the parallel operation

		space.sync(*prefixSum);
		prefixSum->execute(1);
		prefixSum->execute(2);

		space.value *= 2;
	}
	prefixSum->execute(3);

	solver->use();
	solver->execute(1); //Sort
}


void AppLayer::Simulate(Merlin::Timestep ts) {

	solver->use();

	GPU_PROFILE(solver_substep_time,
		for (int i = 0; i < settings.solver_substep; i++) {
			solver->execute(2);
			GPU_PROFILE(nns_time,
				NeigborSearch();
			)

				if (integrate) {
					GPU_PROFILE(jacobi_time,
						for (int j = 0; j < settings.solver_iteration; j++) {
							solver->execute(3);
							solver->execute(4);
						}
					)
						solver->execute(5);

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



void AppLayer::onImGuiRender() {
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
		if (transparency) particles->setDisplayMode(ParticleSystemDisplayMode::POINT_SPRITE_SHADED);
		else particles->setDisplayMode(ParticleSystemDisplayMode::POINT_SPRITE);
	}

	static bool showbed = true;
	if (ImGui::Checkbox("Show build plate", &showbed)) {
		auto bedRef = scene.getChild("bed");
		if (bedRef) {
			if (showbed) scene.getChild("bed")->show();
			else scene.getChild("bed")->hide();
		}
	}

	ImGui::Checkbox("Integrate", &integrate);

	if (ImGui::SmallButton("Reset simulation")) {
		ResetSimulation();
	}

	static bool Pstate = true;
	if (ImGui::Checkbox("Show Particles", &Pstate)) {
		if (Pstate) particles->show();
		else particles->hide();
	}

	static bool Bstate = false;
	if (ImGui::Checkbox("Show Bins", &Bstate)) {
		if (Bstate) bins->show();
		else bins->hide();
	}

	static bool BBstate = false;
	if (ImGui::Checkbox("Show Boundaries", &BBstate)) {
		particleShader->use();
		particleShader->setInt("showBoundary", BBstate);
	}

	static bool Whirlpool = false;
	if (ImGui::Checkbox("whirlpool", &Whirlpool)) {
		solver->use();
		solver->setInt("whirlpool", Whirlpool);
	}

	ImGui::DragInt("Solver substep", &settings.solver_substep, 1, 1, 200);
	ImGui::DragInt("Solver iteration", &settings.solver_iteration, 1, 1, 200);

	if (ImGui::DragFloat3("Camera position", &model_matrix_translation.x, -100.0f, 100.0f)) {
		camera->setPosition(model_matrix_translation);
	}

	if (ImGui::InputFloat("Time step", &settings.timestep.value(), 0.0, 0.02f)) {
		solver->use();
		solver->setFloat("dt", settings.timestep.value());
	}



	if (ImGui::SliderFloat("Fluid particle mass", &settings.particleMass.value(), 0.1, 2.0)) {
		solver->use();
		settings.particleMass.sync(*solver);
	}
	if (ImGui::SliderFloat("Rest density", &settings.restDensity.value(), 0.0, 2.0)) {
		solver->use();
		settings.restDensity.sync(*solver);
		particleShader->use();
		settings.restDensity.sync(*particleShader);
	}
	if (ImGui::SliderFloat("Pressure multiplier", &settings.artificialPressureMultiplier.value(), 0.0, 10.0)) {
		solver->use();
		solver->setFloat("artificialPressureMultiplier", settings.artificialPressureMultiplier.value() * 0.001);
	}
	if (ImGui::SliderFloat("Viscosity", &settings.artificialViscosityMultiplier.value(), 0.0, 1000.0)) {
		solver->use();
		solver->setFloat("artificialViscosityMultiplier", settings.artificialViscosityMultiplier.value()*0.001);
	}

	static float whirlpool = 50;
	if (ImGui::SliderFloat("Whirlpool", &whirlpool, 0.0, 1000.0)) {
		solver->use();
		solver->setFloat("whirlpoolIntensity", whirlpool);
	}

	static int colorMode = 4;
	static const char* options[] = { "Solid color", "Bin index", "Density", "Temperature", "Velocity", "Mass", "Neighbors" };
	if (ImGui::ListBox("Colored field", &colorMode, options, 7)) {
		particleShader->use();
		particleShader->setInt("colorCycle", colorMode);
		binShader->use();
		binShader->setInt("colorCycle", colorMode);
	}

	static int particleTest = 50;
	static int binTest = 1459;
	if (colorMode == 6) {
		if (ImGui::DragInt("Particle to test", &particleTest)) {
			particleShader->use();
			particleShader->setUInt("particleTest", particleTest);
			binShader->use();
			binShader->setUInt("particleTest", particleTest);
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
			binShader->use();
			binShader->setUInt("binTest", binTest);
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
		solver->use();
		solver->execute(stepSolver);
	}

	ImGui::LabelText("PrefixSum", "");
	static int stepPrefix = 0;
	ImGui::LabelText("Solver", "");
	ImGui::SliderInt("PrefixSum Step", &stepPrefix, 0.0, 3.0f);
	if (ImGui::Button("Execute PrefixSum Step")) {
		prefixSum->use();
		prefixSum->execute(stepPrefix);
	}

	if (ImGui::Button("Step Simulation")) {
		GPU_PROFILE(solver_total_time,
			Simulate(0.016);
		)
	}

	if (ImGui::Button("Debug")) {
		throw("DEBUG");
		Console::info() << "DEBUG" << Console::endl;
	}
	ImGui::End();
}
