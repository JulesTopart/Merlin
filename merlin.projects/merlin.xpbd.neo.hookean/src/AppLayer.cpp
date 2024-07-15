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
	glm::vec4 position;
	glm::vec4 pposition;
	glm::vec4 velocity;
};


void AppLayer::onAttach() {
	Layer3D::onAttach();
	camera().setNearPlane(0.5);
	camera().setFarPlane(2000.0);
	camera().translate(glm::vec3(0, -300, 50));
	camera().rotate(glm::vec3(20, 0, 90));

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

	PROFILE_END(total_start_time, total_time);
	PROFILE_BEGIN(total_start_time);

	GPU_PROFILE(render_time,
		renderer.clear();
		renderer.renderScene(scene, camera());
	)

	ps->detach(particleShader);


	if (!paused) {
		GPU_PROFILE(solver_total_time,
			Simulate(0.00016);
		)
	}
}



void AppLayer::SyncUniforms() {

	solver->use();
	
	solver->setUInt("numConstraints", numConstraints);

	integrator->use();
	settings.particleMass.sync(*integrator);
	integrator->setUInt("numParticles", numParticles);
	integrator->setUInt("numConstraints", numConstraints);

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

	particleShader = Shader::create("particle", "assets/shaders/particle.vert", "assets/shaders/particle.frag");
	particleShader->setVec3("lightPos", glm::vec3(0, -200, 1000));


	particleShader->use();
	particleShader->setInt("colorCycle", 4);

	renderer.addShader(particleShader);

	scene.add(Primitives::createFloor(20,10));


	object = ModelLoader::loadMesh("./assets/common/models/bunny.stl");
	object->translate(glm::vec3(0, 0, 30));
	object->scale(5);
	//object = Primitives::createCube(20);
	
	object->setMaterial("red plastic");
	//scene.add(object);

	object->computeBoundingBox();
	BoundingBox bb = object->getBoundingBox();

	glm::vec3 c_size = bb.max - bb.min;
	auto b_box = Primitives::createCube(c_size.x, c_size.y, c_size.z);
	b_box->enableWireFrameMode();
	c_size *= 0.5;
	b_box->translate(bb.min + c_size);
	scene.add(b_box);

	scene.add(TransformObject::create("origin"));
}

void AppLayer::InitPhysics() {

	//Compute Shaders
	solver = StagedComputeShader::create("solver", "assets/shaders/solver/solver.comp", 2);
	integrator = StagedComputeShader::create("integrator", "assets/shaders/solver/integrator.comp", 2);

	//create particle system
	ps = ParticleSystem::create("ParticleSystem", settings.pThread);
	ps->setShader(particleShader);
	ps->setDisplayMode(ParticleSystemDisplayMode::POINT_SPRITE);

	solver->SetWorkgroupLayout(settings.pWkgCount);

	//attach Buffers
	ps->setShader(particleShader);
	ps->addProgram(integrator);
	ps->addField<glm::vec4>("PositionBuffer");
	ps->addField<glm::vec4>("PredictedPositionBuffer");
	ps->addField<glm::vec4>("VelocityBuffer");
	ps->addBuffer<Constraint>("ConstraintBuffer", settings.cThread);
	ps->solveLink(integrator);//test binding points
	ps->detach(integrator);//test binding points

	//ps->addProgram(solver);
	//ps->addBuffer<Constraint>("ConstraintBuffer", settings.cThread);

	//ps->solveLink(solver);//test binding points
	//ps->detach(solver);//test binding points


	ps->link(particleShader->name(), "PositionBuffer");
	ps->link(particleShader->name(), "PredictedPositionBuffer");
	ps->link(particleShader->name(), "VelocityBuffer");

	ps->solveLink(particleShader);//test binding points
	ps->detach(particleShader);//test binding points


	scene.add(ps);
}


void AppLayer::ResetSimulation() {
	elapsedTime = 0;
	Console::info() << "Generating particles..." << Console::endl;

	float spacing = settings.particleRadius * 2.0;
	auto cpu_position = std::vector<glm::vec4>();
	auto cpu_constraints = std::vector<Constraint>();



	numParticles = 0;


	/*
	{
		object->computeBoundingBox();
		object->voxelize(spacing);
		BoundingBox aabb = object->getBoundingBox();
		glm::vec3 bb_size = aabb.max - aabb.min;
		int gridSizeX = ceil(bb_size.x / spacing);
		int gridSizeY = ceil(bb_size.y / spacing);
		int gridSizeZ = ceil(bb_size.z / spacing);

		for (int i = 0; i < gridSizeX * gridSizeY * gridSizeZ; i++) {
			if (object->getVoxels()[i] != 0) {

				int index = i;
				int vz = index / (gridSizeX * gridSizeY);
				index -= (vz * gridSizeX * gridSizeY);
				int vy = index / gridSizeX;
				int vx = index % gridSizeX;

				float x = aabb.min.x + (vx + 0.5) * spacing;
				float y = aabb.min.y + (vy + 0.5) * spacing;
				float z = aabb.min.z + (vz + 0.5) * spacing;

				cpu_position.push_back(glm::vec4(x, y, z, 0.0));
				numParticles++;
			}
		}
	}*/

	/**/
	Mesh_Ptr cube = Primitives::createCube(10);
	cube->calculateIndices();
	cube->removeUnusedVertices();
	/**/

	//Mesh_Ptr cube = Primitives::createSphere(10);

	Indices ids = cube->getIndices();
	Vertices vrts = cube->getVertices();

	for (auto& vtx : vrts) {
		glm::vec4 v = glm::vec4(vtx.position, 1.0);
		cpu_position.push_back(v + glm::vec4(0,0,10, 0));
		numParticles++;
	}

	int ia = 0;
	int ib = 0;
	for (auto& va : vrts) {
		for (auto& vb : vrts) {
			if (ia == ib) continue;
			numConstraints++;

			glm::vec4 va = glm::vec4(vrts[ia].position, 1);
			glm::vec4 vb = glm::vec4(vrts[ib].position, 1);

			Constraint c;
			c.a = ia;
			c.b = ib;
			c.lambda = 0;
			c.restLength = glm::distance(va, vb);
			cpu_constraints.push_back(c);
			ib++;
		}
		ib = 0;
		ia++;
	}


	Console::info() << "Uploading buffer on device..." << Console::endl;

	settings.pThread = numParticles;
	settings.pWkgCount = (settings.pThread + settings.pWkgSize - 1) / settings.pWkgSize; //Total number of workgroup needed

	settings.cThread = numConstraints;
	settings.cWkgCount = (settings.cThread + settings.cWkgSize - 1) / settings.cWkgSize; //Total number of workgroup needed


	integrator->SetWorkgroupLayout(settings.pWkgCount);
	ps->setInstancesCount(settings.pThread);

	solver->SetWorkgroupLayout(settings.cWkgCount);
	

	SyncUniforms();

	Console::info() << "Uploading buffer on device..." << Console::endl;

	ps->writeField("PositionBuffer", cpu_position);
	ps->writeField("PredictedPositionBuffer", cpu_position);
	ps->clearField("VelocityBuffer");

	ps->writeBuffer("ConstraintBuffer", cpu_constraints);
	
}


void AppLayer::Simulate(Merlin::Timestep ts) {

	ps->solveLink(integrator);
	integrator->use();

	float h = ts / float(settings.solver_substep);

	integrator->setFloat("h", h);


	for (int i = 0; i < settings.solver_substep; i++) {
		integrator->execute(0); //predict position and clear lambda

		for (int j = 0; j < settings.solver_iteration; j++) {
			integrator->execute(1); //project constraints
		}

		integrator->execute(2);
		
	}
	ps->detach(integrator);

	/* //constraint centric

	//ps->clearBuffer("ConstraintBuffer");
	float h = ts / float(settings.solver_substep);
	for (int i = 0; i < settings.solver_substep; i++) {

		ps->solveLink(integrator);
		integrator->use();
		integrator->setFloat("h", h); //Spawn particle after prediction

		integrator->execute(0); //predict position
		ps->detach(integrator);


		ps->solveLink(solver);
		solver->use();
		solver->setFloat("h", h); //Spawn particle after prediction

		solver->execute(0); //clear lambda
		for (int j = 0; j < settings.solver_iteration; j++) {
			solver->execute(1); //project constraints
		}
		ps->detach(solver);


		ps->solveLink(integrator);
		integrator->use();
		integrator->execute(2);
		ps->detach(integrator);
	}


	/**/

	
	elapsedTime += settings.timestep.value();

}

void AppLayer::onImGuiRender() {
	//ImGui::ShowDemoWindow();
	ImGui::DockSpaceOverViewport((ImGuiViewport*)0, ImGuiDockNodeFlags_PassthruCentralNode);
	ImGui::Begin("Infos");


	ImGui::LabelText(std::to_string(numParticles).c_str(), "particles");
	ImGui::LabelText(std::to_string(elapsedTime).c_str(), "s");

	ImGui::LabelText("FPS", std::to_string(fps()).c_str());

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
		if (transparency) ps->setDisplayMode(ParticleSystemDisplayMode::POINT_SPRITE_SHADED);
		else ps->setDisplayMode(ParticleSystemDisplayMode::POINT_SPRITE);
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
		if (Pstate) ps->show();
		else ps->hide();
	}

	static bool BBstate = false;
	if (ImGui::Checkbox("Show Boundaries", &BBstate)) {
		particleShader->use();
		particleShader->setInt("showBoundary", BBstate);
	}


	ImGui::DragInt("Solver substep", &settings.solver_substep, 1, 1, 200);
	ImGui::DragInt("Solver iteration", &settings.solver_iteration, 1, 1, 200);

	if (ImGui::DragFloat3("Camera position", &model_matrix_translation.x, -100.0f, 100.0f)) {
		camera().setPosition(model_matrix_translation);
	}

	if (ImGui::SliderFloat("Fluid particle mass", &settings.particleMass.value(), 0.1, 2.0)) {
		solver->use();
		settings.particleMass.sync(*solver);
		integrator->use();
		settings.particleMass.sync(*integrator);
	}
	

	static int colorMode = 4;
	static const char* options[] = { "Solid color", "Bin index", "Density", "Temperature", "Velocity", "Mass", "Neighbors" };
	if (ImGui::ListBox("Colored field", &colorMode, options, 7)) {
		particleShader->use();
		particleShader->setInt("colorCycle", colorMode);
	}

	static int particleTest = 50;
	static int binTest = 1459;
	if (colorMode == 6) {
		if (ImGui::DragInt("Particle to test", &particleTest)) {
			particleShader->use();
			particleShader->setUInt("particleTest", particleTest);
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
