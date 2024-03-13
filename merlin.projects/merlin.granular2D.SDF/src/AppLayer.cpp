#include "AppLayer.h"

#include <GLFW/glfw3.h>
#include <iostream>
#include <iomanip>
#include <algorithm>

using namespace Merlin;

#define PROFILE(VAR, CODE) double start_ ## VAR ## _time = glfwGetTime(); CODE VAR = (glfwGetTime() - start_ ## VAR ## _time)*1000.0;
#define GPU_PROFILE(VAR, CODE) double start_ ## VAR ## _time = glfwGetTime(); CODE glFinish(); VAR = (glfwGetTime() - start_ ## VAR ## _time)*1000.0;

#define PROFILE_BEGIN(STARTVAR) STARTVAR = glfwGetTime();
#define PROFILE_END(STARTVAR, VAR) VAR = (glfwGetTime() - STARTVAR)*1000.0

Mesh_Ptr generateMesh(std::vector<glm::vec2> m_surface) {
	Mesh_Ptr m;
	Vertices vertices;
	Indices indices;

	for (glm::vec2& v : m_surface) {
		indices.push_back(vertices.size());
		vertices.push_back({ glm::vec3(v.x, v.y, 0), glm::vec3(0), glm::vec3(0.75164, 0.60648, 0.22648) });
	}
	indices.push_back(0);

	m = Mesh::create("nozzle", vertices, indices);
	m->setDrawMode(GL_LINE_STRIP);
	return m;
}

AppLayer::AppLayer(){
	Window* w = &Application::get().getWindow();
	int height = w->getHeight();
	int width = w->getWidth();
	camera = createShared<Camera>(width, height, Projection::Orthographic);
	camera->setNearPlane(0.0f);
	camera->setFarPlane(10.0f);
	camera->translate(glm::vec3(0, 0, 1));
	cameraController = createShared<CameraController2D>(camera);
	cameraController->setZoomLevel(250);
	cameraController->setCameraSpeed(100);
}

AppLayer::~AppLayer(){}

void AppLayer::onAttach(){
	enableGLDebugging();
	//ImGui::LoadIniSettingsFromDisk("imgui.ini");
	Console::setLevel(ConsoleLevel::_TRACE);
	glfwSwapInterval(0);

	InitGraphics();
	InitPhysics();

	particleShader->use();
	particleShader->attach(*positionBuffer);
	particleShader->attach(*predictedPositionBuffer);
	particleShader->attach(*velocityBuffer);
	particleShader->attach(*temperatureBuffer);
	particleShader->attach(*metaBuffer);

	binShader->use();
	binShader->attach(*positionBuffer);
	binShader->attach(*predictedPositionBuffer);
	binShader->attach(*velocityBuffer);
	binShader->attach(*temperatureBuffer);
	binShader->attach(*metaBuffer);
	binShader->attach(*binBuffer);

	ResetSimulation();
}

void AppLayer::onDetach(){}

void AppLayer::onEvent(Event& event){
	camera->onEvent(event);
	cameraController->onEvent(event);

	if (event.getEventType() == EventType::MouseScrolled) {
		particleShader->use();
		particleShader->setFloat("zoomLevel", camera->getZoom());
	}
}

float t = 0.0;

void AppLayer::onUpdate(Timestep ts){
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
	particleShader->setUInt("numParticles",numParticles);
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

	/*
	particleBuffer->bind();
	particleBuffer->resize(settings.pThread);
	particleBuffer->unbind();

	particleCpyBuffer->bind();
	particleCpyBuffer->resize(settings.pThread);
	particleCpyBuffer->unbind();

	sortedIndexBuffer->bind();
	sortedIndexBuffer->resize(settings.pThread);
	sortedIndexBuffer->unbind();

	binBuffer->bind();
	binBuffer->resize(settings.bThread);
	binBuffer->unbind();
	*/

	particleSystem->setInstancesCount(settings.pThread);
	binSystem->setInstancesCount(settings.bThread);
	
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
	particleShader->setVec3("lightPos", glm::vec3(0, -200, 1000));

	binShader = Shader::create("bins", "assets/shaders/bin.vert", "assets/shaders/bin.frag");
	binShader->noTexture();
	binShader->noMaterial();

	particleShader->use();
	particleShader->setInt("colorCycle", 3);
	binShader->use();
	binShader->setInt("colorCycle", 3);
	 
	renderer.addShader(particleShader);
	renderer.addShader(binShader);

	//qrenderer.setShader(Shader("screen", "assets/shaders/screen.space.vert", "assets/shaders/screen.space.frag"));

	Model_Ptr floor = Model::create("floor", Primitives::createLine(1000));
	floor->enableWireFrameMode();
	floor->translate(glm::vec3(-1000 /2.0, -settings.bb.y / 2.0, 0.0));
	scene.add(floor);

	std::vector<glm::vec2> geomr = {
		glm::vec2(2.5,0),
		glm::vec2(2.5,10),
		glm::vec2(50,50),
		glm::vec2(50,120),
		glm::vec2(75,120),
		glm::vec2(75,30),
		glm::vec2(50.0, 0),
		glm::vec2(2.5, 0),
	};

	std::vector<glm::vec2> geoml = {
		glm::vec2(-2.5,0),
		glm::vec2(-2.5,10),
		glm::vec2(-50,50),
		glm::vec2(-50,120),
		glm::vec2(-75,120),
		glm::vec2(-75,30),
		glm::vec2(-50.0, 0),
		glm::vec2(-2.5, 0),
	};


	Model_Ptr temp = Model::create("nozzle", generateMesh(geomr));
	temp->translate(glm::vec3(0, -100,0));
	scene.add(temp);
	temp = Model::create("nozzle", generateMesh(geoml));
	temp->translate(glm::vec3(0, -100, 0));
	scene.add(temp);
	scene.add(TransformObject::create("origin"));

}

void AppLayer::InitPhysics() {
	//Compute Shaders
	solver = StagedComputeShader::create("solver", "assets/shaders/solver/solver.comp", 6);
	prefixSum = StagedComputeShader::create("prefixSum", "assets/shaders/solver/prefix.sum.comp", 4);

	//create particle system
	particleSystem = deprecated_ParticleSystem::create("ParticleSystem", settings.pThread);
	Shared<Mesh> particle = Primitives::createPoint();
	particle->rename("particle");
	particle->setShader(particleShader);
	particleSystem->setMesh(particle);
	particleSystem->setDisplayMode(deprecated_ParticleSystemDisplayMode::POINT_SPRITE);

	Shared<Mesh> binInstance = Primitives::createQuadRectangle(settings.bWidth, settings.bWidth, true);
	binInstance->rename("bin");
	binInstance->setShader(binShader);
	binSystem = deprecated_ParticleSystem::create("BinSystem", settings.bThread);
	binSystem->setDisplayMode(deprecated_ParticleSystemDisplayMode::MESH);
	binSystem->setMesh(binInstance);
	binSystem->enableWireFrameMode();

	solver->SetWorkgroupLayout(settings.pWkgCount);
	prefixSum->SetWorkgroupLayout(settings.bWkgCount);

	// reserve Buffer and double buffering
	positionBuffer = SSBO<glm::vec2>::create("PositionBuffer", settings.pThread);
	cpyPositionBuffer = SSBO<glm::vec2>::create("cpyPositionBuffer",settings.pThread);
	predictedPositionBuffer = SSBO<glm::vec2>::create("PredictedPositionBuffer",settings.pThread);
	cpyPredictedPositionBuffer = SSBO<glm::vec2>::create("cpyPredictedPositionBuffer",settings.pThread);
	velocityBuffer = SSBO<glm::vec2>::create("VelocityBuffer",settings.pThread);
	cpyVelocityBuffer = SSBO<glm::vec2>::create("cpyVelocityBuffer",settings.pThread);
	temperatureBuffer = SSBO<float>::create("TemperatureBuffer", settings.pThread);
	cpyTemperatureBuffer = SSBO<float>::create("cpyTemperatureBuffer",settings.pThread);
	metaBuffer = SSBO<glm::uvec4>::create("MetaBuffer",settings.pThread);
	cpymetaBuffer = SSBO<glm::uvec4>::create("cpyMetaBuffer",settings.pThread);


	Console::info() << "Bin struct size :" << sizeof(Bin) << Console::endl;
	binBuffer = SSBO<Bin>::create("BinBuffer",settings.bThread);


	// Set binding points for position and its copy
	positionBuffer->setBindingPoint(0);
	cpyPositionBuffer->setBindingPoint(1);
	predictedPositionBuffer->setBindingPoint(2);
	cpyPredictedPositionBuffer->setBindingPoint(3);
	velocityBuffer->setBindingPoint(4);
	cpyVelocityBuffer->setBindingPoint(5);
	temperatureBuffer->setBindingPoint(6);
	cpyTemperatureBuffer->setBindingPoint(7);
	metaBuffer->setBindingPoint(9);
	cpymetaBuffer->setBindingPoint(10);
	binBuffer->setBindingPoint(11);

	//attach Buffers
	particleSystem->addComputeShader(solver);
	particleSystem->addStorageBuffer(positionBuffer);
	particleSystem->addStorageBuffer(cpyPositionBuffer);
	particleSystem->addStorageBuffer(predictedPositionBuffer);
	particleSystem->addStorageBuffer(cpyPredictedPositionBuffer);
	particleSystem->addStorageBuffer(velocityBuffer);
	particleSystem->addStorageBuffer(cpyVelocityBuffer);
	particleSystem->addStorageBuffer(temperatureBuffer);
	particleSystem->addStorageBuffer(cpyTemperatureBuffer);
	particleSystem->addStorageBuffer(metaBuffer);
	particleSystem->addStorageBuffer(cpymetaBuffer);
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
	
	Console::info() << "Generating particles..." << Console::endl;

	float spacing = settings.particleRadius * 2.0;

	auto cpu_position = positionBuffer->getEmptyArray();
	auto cpu_predictedPosition = predictedPositionBuffer->getEmptyArray();
	auto cpu_velocity = velocityBuffer->getEmptyArray();
	auto cpu_temperature = temperatureBuffer->getEmptyArray();
	auto cpu_meta = metaBuffer->getEmptyArray();

	glm::vec2 cubeSize = glm::vec2(90, 125);
	glm::ivec2 icubeSize = glm::vec2(cubeSize.x / spacing, cubeSize.y / spacing);

	numParticles = 0;
	for (int yi = -90; yi <= (cubeSize.y / 2.0) / spacing; yi++)
	for (int xi = -(cubeSize.x / 2.0) / spacing; xi <= (cubeSize.x / 2.0) / spacing; xi++){
		float x = ((xi + 1) * spacing);
		float y = ((yi + 1) * spacing);
		cpu_position.push_back(glm::vec2(x, y));
		cpu_predictedPosition.push_back(glm::vec2(x, y));
		cpu_velocity.push_back(glm::vec2(0));
		cpu_temperature.push_back(215 + 298.15);
		cpu_meta.push_back(glm::uvec4(GRANULAR, numParticles, numParticles, 0.0));
		numParticles++;
	}


	Console::info() << "Uploading buffer on device..." << Console::endl;
	settings.pThread = numParticles;
	particleSystem->setInstancesCount(settings.pThread);
	
	ApplyBufferSettings();
	SyncUniforms();

	positionBuffer->write(cpu_position);
	predictedPositionBuffer->write(cpu_predictedPosition);
	velocityBuffer->write(cpu_velocity);
	temperatureBuffer->write(cpu_temperature);
	metaBuffer->write(cpu_meta);

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

	static int colorMode = 3;
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
