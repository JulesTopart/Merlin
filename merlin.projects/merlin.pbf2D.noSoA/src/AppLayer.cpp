#include "Applayer.h"

#include <GLFW/glfw3.h>
#include <iostream>
#include <iomanip>
#include <algorithm>

using namespace Merlin;


#define PROFILE(VAR, CODE) double start_ ## VAR ## _time = glfwGetTime(); CODE VAR = (glfwGetTime() - start_ ## VAR ## _time)*1000.0;
#define GPU_PROFILE(VAR, CODE) double start_ ## VAR ## _time = glfwGetTime(); CODE glFinish(); VAR = (glfwGetTime() - start_ ## VAR ## _time)*1000.0;

#define PROFILE_BEGIN(STARTVAR) STARTVAR = glfwGetTime();
#define PROFILE_END(STARTVAR, VAR) VAR = (glfwGetTime() - STARTVAR)*1000.0

AppLayer::AppLayer(){
	Window* w = &Application::get().getWindow();
	int height = w->getHeight();
	int width = w->getWidth();
	camera = createShared<Camera>(width, height, Projection::Orthographic);
	camera->setNearPlane(0.0f);
	camera->setFarPlane(10.0f);
	camera->translate(glm::vec3(0, 0, 1));
	cameraController = createShared<CameraController2D>(camera);
	cameraController->setZoomLevel(20);
	cameraController->setCameraSpeed(100);
}

AppLayer::~AppLayer(){}

void AppLayer::onAttach(){
	Layer2D::onAttach();

	glfwSwapInterval(0);

	InitGraphics();
	InitPhysics();

	ResetSimulation();
}

void AppLayer::onDetach(){}

void AppLayer::onEvent(Event& event){
	Layer2D::onEvent(event);

	if (event.getEventType() == EventType::MouseScrolled) {
		particleShader->use();
		particleShader->setFloat("zoomLevel", camera().getZoom());
	}
	else if (event.getEventType() == EventType::WindowResize) {
		particleShader->use();
		particleShader->setVec2("WindowSize", glm::vec2(camera().width(), camera().height()));
	}
}


void AppLayer::onUpdate(Timestep ts){
	Layer2D::onUpdate(ts);

	PROFILE_END(total_start_time, total_time);
	PROFILE_BEGIN(total_start_time);

	GPU_PROFILE(render_time,
		renderer.clear();
		renderer.renderScene(scene, camera());
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
	particleShader->setUInt("numParticles",numParticles);
	settings.restDensity.sync(*particleShader);

	prefixSum->use();
	prefixSum->setUInt("dataSize", settings.bThread); //data size
	prefixSum->setUInt("blockSize", settings.blockSize); //block size
}


void AppLayer::InitGraphics() {
	// init OpenGL stuff
	renderer.initialize();
	renderer.setBackgroundColor(0.203, 0.203, 0.203, 1.0);
	renderer.disableEnvironment();
	renderer.enableTransparency();
	renderer.enableSampleShading();
	renderer.disableShadows();

	particleShader = Shader::create("particle", "assets/shaders/particle.vert", "assets/shaders/particle.frag");
	particleShader->setVec3("lightPos", glm::vec3(0, -200, 1000));
	particleShader->noEnvironment();
	particleShader->noMaterial();
	particleShader->noTexture();
	particleShader->noLights();
	particleShader->noShadows();

	binShader = Shader::create("bins", "assets/shaders/bin.vert", "assets/shaders/bin.frag");
	binShader->noEnvironment();
	binShader->noMaterial();
	binShader->noTexture();
	binShader->noLights();
	binShader->noShadows();

	particleShader->use();
	particleShader->setInt("colorCycle", 3);
	particleShader->setVec2("WindowSize", glm::vec2(camera().width(), camera().height()));

	binShader->use();
	binShader->setInt("colorCycle", 3);
	 
	renderer.addShader(particleShader);
	renderer.addShader(binShader);

	//qrenderer.setShader(Shader("screen", "assets/shaders/screen.space.vert", "assets/shaders/screen.space.frag"));

	Model_Ptr mdl = Model::create("bbox", Primitives::createQuadRectangle(settings.bb.x, settings.bb.y, true));
	mdl->enableWireFrameMode();
	scene.add(mdl);

	//scene.Add(TransformObject::create("origin"));

}

void AppLayer::InitPhysics() {
	//Compute Shaders
	solver = StagedComputeShader::create("solver", "assets/shaders/solver/solver.comp", 6);
	prefixSum = StagedComputeShader::create("prefixSum", "assets/shaders/solver/prefix.sum.comp", 4);

	//create particle system
	ps = ParticleSystem::create("ParticleSystem", settings.pThread);
	ps->setShader(particleShader);
	ps->setDisplayMode(ParticleSystemDisplayMode::POINT_SPRITE);

	Shared<Mesh> binInstance = Primitives::createQuadRectangle(settings.bWidth, settings.bWidth, true);
	binInstance->rename("bin");
	binInstance->setShader(binShader);
	bs = ParticleSystem::create("BinSystem", settings.bThread);
	bs->setDisplayMode(ParticleSystemDisplayMode::MESH);
	bs->setMesh(binInstance);
	bs->enableWireFrameMode();

	solver->SetWorkgroupLayout(settings.pWkgCount);
	prefixSum->SetWorkgroupLayout(settings.bWkgCount);



	//reserve Buffers
	Console::info() << "Particle struct size :" << sizeof(Particle) << Console::endl;
	Console::info() << "Bin struct size :" << sizeof(Bin) << Console::endl;
	SSBO_Ptr<Bin> binBuffer = SSBO<Bin>::create("BinBuffer",settings.bThread);

	particleBuffer->setBindingPoint(0);
	particleCpyBuffer->setBindingPoint(1);
	sortedIndexBuffer->setBindingPoint(2);
	binBuffer->setBindingPoint(3);

	//attach Buffers
	particleSystem->addComputeShader(solver);
	particleSystem->addStorageBuffer(particleBuffer);
	particleSystem->addStorageBuffer(particleCpyBuffer);
	particleSystem->addStorageBuffer(sortedIndexBuffer);
	particleSystem->addStorageBuffer(binBuffer);

	binSystem->addComputeShader(prefixSum);
	binSystem->addStorageBuffer(binBuffer);
	
	scene.add(particleSystem);
	scene.add(binSystem);
	//scene.Add(constraintSystem);
	binSystem->hide();
}

void AppLayer::ResetSimulation() {
	elapsedTime = 0;
	particleBuffer->clear();
	
	Console::info() << "Generating particles..." << Console::endl;

	float spacing = settings.particleRadius * 2.0;
	auto cpu_particles = particleBuffer->getEmptyArray();

	Particle buf;
	buf.velocity = glm::vec2(0);
	
	buf.phase = FLUID; // phase
	glm::vec2 cubeSize = glm::vec2(100, 250);
	glm::ivec2 icubeSize = glm::vec2(cubeSize.x / spacing, cubeSize.y / spacing);

	for (int yi = 0; yi <= cubeSize.y / spacing; yi++)
		for (int xi = 0; xi <= cubeSize.x / spacing; xi++){
				float x = ((xi + 1) * spacing) - (settings.bb.x/2.0);
				float y = ((yi + 1) * spacing) - (settings.bb.y/2.0);
				buf.id = 0;// cpu_particles.size();
				buf.position.x = x;
				buf.position.y = y;
				buf.pposition = buf.position;
				buf.binIndex = cpu_particles.size();
				cpu_particles.push_back(buf);
			}

	/*
	buf.phase = BOUNDARY; // phase
	for (int xi = 0; xi <= settings.bb.x / spacing; xi++) {
		float x = (xi * spacing) - (settings.bb.x / 2.0);
		buf.id = cpu_particles.size();
		buf.position.x = x;
		buf.position.y = -(settings.bb.y / 2.0);
		buf.pposition = buf.position;
		buf.binIndex = cpu_particles.size();
		cpu_particles.push_back(buf);

		buf.position.y = settings.bb.y/2.0;
		buf.pposition = buf.position;
		buf.binIndex = cpu_particles.size();
		cpu_particles.push_back(buf);
	}

	for (int yi = 0; yi <= settings.bb.y / spacing; yi++) {
		float y = (yi * spacing) - (settings.bb.y / 2.0);
		buf.id = cpu_particles.size();
		buf.position.x = -(settings.bb.x / 2.0);
		buf.position.y = y;
		buf.pposition = buf.position;
		buf.binIndex = cpu_particles.size();
		cpu_particles.push_back(buf);

		buf.position.x = settings.bb.x/2.0;
		buf.pposition = buf.position;
		buf.binIndex = cpu_particles.size();
		cpu_particles.push_back(buf);
	}*/
	
	Console::info() << "Uploading buffer on device..." << Console::endl;

	numParticles = cpu_particles.size();
	settings.pThread = numParticles;
	particleSystem->setInstancesCount(settings.pThread);
	
	ApplyBufferSettings();
	SyncUniforms();
	
	particleBuffer->bind();
	particleBuffer->write(cpu_particles);
	particleBuffer->unbind();
}


void AppLayer::NeigborSearch() {
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
	if (ImGui::Checkbox("Particle transparency", &transparency)) {
		if (transparency) particleSystem->setDisplayMode(deprecated_ParticleSystemDisplayMode::POINT_SPRITE_SHADED);
		else particleSystem->setDisplayMode(deprecated_ParticleSystemDisplayMode::POINT_SPRITE);
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
		if (Pstate) particleSystem->show();
		else particleSystem->hide();
	}

	static bool Bstate = false;
	if (ImGui::Checkbox("Show Bins", &Bstate)) {
		if (Bstate) binSystem->show();
		else binSystem->hide();
	}

	static bool BBstate = false;
	if (ImGui::Checkbox("Show Boundaries", &BBstate)) {
		particleShader->use();
		particleShader->setInt("showBoundary", BBstate);
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



	if(ImGui::SliderFloat("Fluid particle mass", &settings.particleMass.value(), 0.1, 2.0)){
		solver->use();
		settings.particleMass.sync(*solver);
	}
	if (ImGui::SliderFloat("Rest density", &settings.restDensity.value(), 0.0, 2.0)){
		solver->use();
		settings.restDensity.sync(*solver);
		particleShader->use();
		settings.restDensity.sync(*particleShader);
	}
	if (ImGui::SliderFloat("Pressure multiplier", &settings.artificialPressureMultiplier.value(), 0.0, 10.0)){
		solver->use();
		solver->setFloat("artificialPressureMultiplier", settings.artificialPressureMultiplier.value() * 0.001);
	}
	if (ImGui::SliderFloat("Viscosity", &settings.artificialViscosityMultiplier.value(), 0.0, 10.0)){
		solver->use();
		solver->setFloat("artificialViscosityMultiplier", settings.artificialViscosityMultiplier.value() * 0.001);
	}

	static int colorMode = 0;
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
