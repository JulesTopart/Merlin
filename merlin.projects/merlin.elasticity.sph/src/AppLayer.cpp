#include "Applayer.h"

#include <GLFW/glfw3.h>
#include <iostream>
#include <iomanip>

using namespace Merlin;

#define PROFILE(VAR, CODE) double start_ ## VAR ## _time = glfwGetTime(); CODE VAR = (glfwGetTime() - start_ ## VAR ## _time)*1000.0;
#define GPU_PROFILE(VAR, CODE) double start_ ## VAR ## _time = glfwGetTime(); CODE glFinish(); VAR = (glfwGetTime() - start_ ## VAR ## _time)*1000.0;

#define PROFILE_BEGIN(STARTVAR) STARTVAR = glfwGetTime();
#define PROFILE_END(STARTVAR, VAR) VAR = (glfwGetTime() - STARTVAR)*1000.0

struct CopyContent {
	glm::vec4 lx;
	glm::vec4 x;
	glm::vec4 p;
	glm::vec4 dp;
	glm::vec4 v;
	float density;
	float pad[3];
	float rotation[12];
	glm::uvec4 meta;
};

void AppLayer::onAttach() {
	Layer3D::onAttach();
	camera().setNearPlane(0.5);
	camera().setFarPlane(2000.0);
	camera().translate(glm::vec3(0, -200, 50));
	camera().rotate(glm::vec3(200, 0, 90));

	glfwSwapInterval(0);

	InitGraphics();
	InitPhysics();

	ResetSimulation();
}

void AppLayer::onDetach() {}

void AppLayer::onEvent(Event& event) {
	Layer3D::onEvent(event);
}

void AppLayer::onUpdate(Timestep ts) {
	Layer3D::onUpdate(ts);

	ps->solveLink(particleShader);
	bs->solveLink(binShader);

	PROFILE_END(total_start_time, total_time);
	PROFILE_BEGIN(total_start_time);

	GPU_PROFILE(render_time,
		renderer.clear();
		renderer.renderScene(scene, camera());
	)

	ps->detach(particleShader);
	bs->detach(binShader);

	ps->solveLink(solver);
	bs->solveLink(prefixSum);

	if (!paused) {
		GPU_PROFILE(solver_total_time,
			Simulate(0);
		)
	}

	ps->detach(solver);
	bs->detach(prefixSum);
}



void AppLayer::SyncUniforms() {

	solver->use();

	//settings.timestep.sync(*solver);
	settings.particleMass.sync(*solver);
	settings.restDensity.sync(*solver);
	settings.numParticles.sync(*solver);
	settings.dt.sync(*solver);

	Console::info() << "Total averaged solid mass (g): " << settings.particleMass() * settings.numParticles() << Console::endl;
	Console::info() << "Total averaged solid volume (mm3): " << settings.restDensity() * pow(settings.particleRadius,3) * settings.numParticles() << Console::endl;

	particleShader->use();
	settings.numParticles.sync(*particleShader);
	settings.restDensity.sync(*particleShader);

	prefixSum->use();
	prefixSum->setUInt("dataSize", settings.bThread); //data size
	prefixSum->setUInt("blockSize", settings.blockSize); //block size
}


void AppLayer::InitGraphics() {
	// init OpenGL stuff
	renderer.initialize();
	renderer.setBackgroundColor(0.803, 0.803, 0.803, 1.0);
	renderer.enableTransparency();
	renderer.enableSampleShading();
	//renderer.disableShadows();
	renderer.disableFaceCulling();
	//renderer.applyGlobalTransform(glm::scale(glm::mat4(1), glm::vec3(0.001)));

	particleShader = Shader::create("particle", "assets/shaders/particle.vert", "assets/shaders/particle.frag", "", false);
	settings.setConstants(*particleShader);
	particleShader->compile();
	particleShader->use();
	particleShader->setVec3("lightPos", glm::vec3(0, -200, 1000));
	particleShader->setInt("colorCycle", 4);

	binShader = Shader::create("bins", "assets/shaders/bin.vert", "assets/shaders/bin.frag", "", false);
	settings.setConstants(*binShader);
	binShader->compile();
	binShader->use();
	binShader->setInt("colorCycle", 4);

	renderer.addShader(particleShader);
	renderer.addShader(binShader);

	//sample = Primitives::createCube(10,10, 80);
	sample = ModelLoader::loadMesh("./assets/models/tensile.D5766.stl");
	//sample->enableWireFrameMode();
	//sample->centerMeshOrigin();
	//sample->rotate(glm::vec3(0,90*DEG_TO_RAD,0));
	sample->applyMeshTransform();
	sample->smoothNormals();
	//scene.add(sample);


	Mesh_Ptr bbox = Primitives::createCube(settings.bb.x, settings.bb.y, settings.bb.z);
	bbox->enableWireFrameMode();
	bbox->setMaterial("red plastic");
	//scene.add(bbox);


	scene.add(TransformObject::create("origin"));
}

void AppLayer::InitPhysics() {

	//Compute Shaders
	solver = StagedComputeShader::create("solver", "assets/shaders/solver/solver.comp", 7, false);
	settings.setConstants(*solver);
	solver->compile();
	
	prefixSum = StagedComputeShader::create("prefixSum", "assets/shaders/solver/prefix.sum.comp", 4, false);
	settings.setConstants(*prefixSum);
	prefixSum->compile();

	//create particle system
	ps = ParticleSystem::create("ParticleSystem", settings.pThread);
	ps->setShader(particleShader);
	ps->setDisplayMode(ParticleSystemDisplayMode::POINT_SPRITE_SHADED);

	Shared<Mesh> binInstance = Primitives::createQuadCube(settings.bWidth, false);
	binInstance->rename("bin");
	bs = ParticleSystem::create("BinSystem", settings.bThread);
	bs->setDisplayMode(ParticleSystemDisplayMode::MESH);
	bs->setMesh(binInstance);
	bs->enableWireFrameMode();

	solver->SetWorkgroupLayout(settings.pWkgCount);
	prefixSum->SetWorkgroupLayout(settings.bWkgCount);

	Console::info() << "Bin struct size :" << sizeof(Bin) << Console::endl;
	SSBO_Ptr<Bin> binBuffer = SSBO<Bin>::create("BinBuffer", settings.bThread);

	//attach Buffers
	ps->setShader(particleShader);
	ps->addProgram(solver);
	ps->addField<glm::vec4>("last_position_buffer");
	ps->addField<glm::vec4>("position_buffer");
	ps->addField<glm::vec4>("predicted_position_buffer");
	ps->addField<glm::vec4>("correction_buffer");
	ps->addField<glm::vec4>("velocity_buffer");
	ps->addField<float>("density_buffer");
	ps->addField<float>("lambda_buffer");
	ps->addField<float[8]>("stress_buffer");
	ps->addField<float[12]>("rotation_buffer");
	ps->addField<glm::uvec4>("meta_buffer");
	ps->addField<CopyContent>("copy_buffer");
	ps->addBuffer(binBuffer);
	ps->solveLink(solver);
	ps->detach(solver);//test binding points

	bs->setShader(binShader);
	bs->addProgram(prefixSum);
	bs->addField(binBuffer);
	bs->solveLink(prefixSum);
	bs->detach(prefixSum);//test binding points

	ps->link(particleShader->name(), "last_position_buffer");
	ps->link(particleShader->name(), "position_buffer");
	ps->link(particleShader->name(), "velocity_buffer");
	ps->link(particleShader->name(), "density_buffer");
	ps->link(particleShader->name(), "stress_buffer");
	ps->link(particleShader->name(), "meta_buffer");
	ps->solveLink(particleShader);
	ps->detach(particleShader);//test binding points

	bs->link(binShader->name(), binBuffer->name());
	bs->solveLink(binShader);
	bs->detach(binShader);//test binding points

	scene.add(ps);
	scene.add(bs);
	scene.add(TransformObject::create("origin", 10));
	bs->hide();
}


float pullDistance = 0;
void AppLayer::ResetSimulation() {
	elapsedTime = 0;
	pullDistance = 0;
	BindingPointManager::instance().resetBindings();

	ps->detach(solver);
	bs->detach(prefixSum);
	ps->detach(particleShader);
	bs->detach(binShader);

	Console::info() << "Generating particles..." << Console::endl;

	float spacing = settings.particleRadius * 2.0;
	auto cpu_position = std::vector<glm::vec4>();
	auto cpu_meta = std::vector<glm::uvec4>();

	settings.numParticles = 0;
	std::vector<glm::vec3> positions;

	
	sample->computeBoundingBox();
	sample->voxelize(spacing);
	positions = Voxelizer::getVoxelposition(sample->getVoxels(), sample->getBoundingBox(), spacing);

	for (int i = 0; i < positions.size(); i++) {
		cpu_position.push_back(glm::vec4(positions[i],0));
		if(positions[i].x < sample->getBoundingBox().min.x + 25) 
			cpu_meta.push_back(glm::uvec4(SOLIDA, settings.numParticles(), settings.numParticles(), 0.0));

		else if(positions[i].x > sample->getBoundingBox().max.x - 25) 
			cpu_meta.push_back(glm::uvec4(SOLIDB, settings.numParticles(), settings.numParticles(), 0.0));

		else 
			cpu_meta.push_back(glm::uvec4(SOLID, settings.numParticles(), settings.numParticles(), 0.0));
		settings.numParticles()++;
	}
	
	Console::info() << "Uploading buffer on device..." << Console::endl;

	settings.pThread = settings.numParticles();
	settings.pWkgCount = (settings.pThread + settings.pWkgSize - 1) / settings.pWkgSize; //Total number of workgroup needed
	settings.blockSize = std::floor(log2f(settings.bThread));
	settings.blocks = (settings.bThread + settings.blockSize - 1) / settings.blockSize;
	settings.bWkgCount = (settings.blocks + settings.bWkgSize - 1) / settings.bWkgSize; //Total number of workgroup needed

	solver->SetWorkgroupLayout(settings.pWkgCount);
	prefixSum->SetWorkgroupLayout(settings.bWkgCount);

	ps->setInstancesCount(settings.pThread);
	ps->setActiveInstancesCount(settings.pThread);
	bs->setInstancesCount(settings.bThread);

	float mass = settings.particleMass() * double(settings.numParticles()) * 1e-3;
	float volume = pow(settings.smoothingRadius, 3.0) * double(settings.numParticles());
	Console::print() << "---------------------------------------------------------" << Console::endl;
	Console::info() << "Total averaged solid mass (g): " << mass << Console::endl;
	Console::info() << "Total averaged solid volume (mm3): " << volume << Console::endl;
	Console::info() << "Density (g/cm3): " << 1e3 * (mass / volume) << Console::endl;

	Console::print() << "u_rho0 (g/cm3) : " << settings.restDensity() << Console::endl;
	Console::print() << "u_dt : " << settings.dt() << Console::endl;
	Console::print() << "u_particleMass (mg) : " << settings.particleMass() << Console::endl;
	Console::print() << "---------------------------------------------------------" << Console::endl;

	SyncUniforms();
	Console::info() << "Uploading buffer on device..." << Console::endl;
	ps->writeField("position_buffer", cpu_position);
	ps->writeField("predicted_position_buffer", cpu_position);
	ps->writeField("last_position_buffer", cpu_position);
	ps->writeField("meta_buffer", cpu_meta);

	ps->clearField("velocity_buffer");
	ps->clearField("density_buffer");
	ps->clearField("lambda_buffer");
	ps->clearField("correction_buffer");
	ps->clearField("stress_buffer");

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
	space.value() = 1;

	for (GLuint step = 0; step < steps; step++) {
		// Calls the parallel operation

		space.sync(*prefixSum);
		prefixSum->execute(1);
		prefixSum->execute(2);

		space.value() *= 2;
	}
	prefixSum->execute(3);

	solver->use();
	solver->execute(1); //Sort
}


static bool firstRun = true;
static bool pullTest = false;

void AppLayer::Simulate(Merlin::Timestep ts) {
	elapsedTime += settings.dt.value();

	solver->use();
	settings.dt.sync(*solver);

	if (pullTest && pullDistance < 20) {
		pullDistance += 1.0 * settings.dt.value();
		solver->setFloat("u_pullDistance", pullDistance);
	}

	GPU_PROFILE(nns_time,
		NeigborSearch();
	)

	if (firstRun) {
		solver->execute(6);
		firstRun = false;
	}

	GPU_PROFILE(solver_time,
		solver->execute(2);
		solver->execute(3);
		solver->execute(4);
		solver->execute(5);
	)
}


void AppLayer::onImGuiRender() {
	//ImGui::ShowDemoWindow();

	ImGui::DockSpaceOverViewport((ImGuiViewport*)0, ImGuiDockNodeFlags_PassthruCentralNode);


	ImGui::Begin("General");
	
	ImGui::Text("time : %f s", elapsedTime);
	ImGui::Text("dL : %f s", 2.0*pullDistance);

	ImGui::Checkbox("Pull", &pullTest);

	
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

	static float maxTime = 0;
	if (ImGui::SmallButton("Reset simulation")) {
		ResetSimulation();
		firstRun = true;
		maxTime = 0;
	}

	ImGui::Separator();

	if (ImGui::TreeNode("Stats"))
	{
		ImGui::Text("particles: %i", settings.numParticles());
		ImGui::Text("bins : %i", settings.bThread);
		ImGui::Text("%i FPS", fps());

		
		if (maxTime < frametime() * 1000.0) maxTime = frametime()*1300.0;
		

		frameTimes.push_back(frametime()*1000.0);
		if (frameTimes.size() > 100) {
			frameTimes.erase(frameTimes.begin());
		}
			
		ImGui::PlotHistogram("Frame time (ms)", frameTimes.data(), frameTimes.size(),0, "", 0.0, maxTime, ImVec2(0, 80));

		ImGui::TreePop();
	}


	if (ImGui::TreeNode("Graphics"))
	{
		if (ImGui::DragFloat3("Camera position", &model_matrix_translation.x, -100.0f, 100.0f)) {
			camera().setPosition(model_matrix_translation);
		}

		static bool transparency = true;
		if (ImGui::Checkbox("Particle transparency", &transparency)) {
			if (transparency) ps->setDisplayMode(ParticleSystemDisplayMode::POINT_SPRITE_SHADED);
			else ps->setDisplayMode(ParticleSystemDisplayMode::POINT_SPRITE);
		}

		static bool Pstate = true;
		if (ImGui::Checkbox("Show Particles", &Pstate)) {
			if (Pstate) ps->show();
			else ps->hide();
		}

		static bool Bstate = false;
		if (ImGui::Checkbox("Show Bins", &Bstate)) {
			if (Bstate) bs->show();
			else bs->hide();
		}

		static bool BBstate = false;
		if (ImGui::Checkbox("Show Boundaries", &BBstate)) {
			particleShader->use();
			particleShader->setInt("showBoundary", BBstate);
		}
		ImGui::TreePop();
	}

	
	if (ImGui::TreeNode("Parameters"))
	{
		static float dt = settings.dt.value() * 1.0e6;
		if (ImGui::InputFloat("Time step (us)", &dt, 0.0, 1000.0f)) {
			settings.dt.value() = dt * 1.0e-6;
		}


		if (ImGui::SliderFloat("Fluid particle mass (g)", &settings.particleMass.value(), 0.0, 1.0)) {
			solver->use();
			settings.particleMass.sync(*solver);
		}

		if (ImGui::SliderFloat("Rest density (g/cm3)", &settings.restDensity.value(), 0.0, 2.0)) {
			solver->use();
			settings.restDensity.sync(*solver);
			particleShader->use();
			settings.restDensity.sync(*particleShader);
		}

		ImGui::TreePop();
	}






	static int colorMode = 4;
	static const char* options[] = { "Solid color", "Bin index", "Density", "Stress", "Velocity", "Mass", "Neighbors" };
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
	ImGui::Text("Solver %.1f ms", solver_time);
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

		std::vector<glm::uvec4> metabuf;
		metabuf.resize(settings.pThread);
		ps->getField("meta_buffer")->readBuffer(settings.pThread*sizeof(glm::uvec4), metabuf.data());

		throw("DEBUG");

		Console::info() << "DEBUG" << Console::endl;
	}
	ImGui::End();
}
