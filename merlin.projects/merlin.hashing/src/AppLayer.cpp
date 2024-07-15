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
	float density;
	float lambda;
	float _padding;
	float _padding2;
	glm::uvec4 meta;
};

void AppLayer::onAttach() {
	Layer3D::onAttach();

	camera().setNearPlane(0.5);
	camera().setFarPlane(2000.0);
	camera().translate(glm::vec3(0, -500, 100));
	camera().rotate(glm::vec3(40, 0, 90));

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

	PROFILE_END(total_start_time, total_time);
	PROFILE_BEGIN(total_start_time);

	ps->solveLink(particleShader);
	bs->solveLink(binShader);

	GPU_PROFILE(render_time,
		renderer.clear();
		renderer.renderScene(scene, camera());
	)

	ps->detach(particleShader);
	bs->detach(binShader);



	if (!paused) {
		ps->solveLink(solver);
		bs->solveLink(prefixSum);

		GPU_PROFILE(solver_total_time,
			Simulate(0.016);
		)

		ps->detach(solver);
		bs->detach(prefixSum);


		/*
		if (use_isosurface) {
			ps->solveLink(isoGen);
			volume->bindImage(0);
			isoGen->use();
			isoGen->dispatch();
			isoGen->barrier(GL_ALL_BARRIER_BITS);
			ps->detach(isoGen);

			isosurface->setIsoLevel(0.1);
			isosurface->compute();
		}*/
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

	isoGen->use();
	isoGen->setUInt("numParticles", numParticles);
	settings.particleMass.sync(*isoGen);
}

void AppLayer::InitGraphics() {
	// init OpenGL stuff
	renderer.initialize();
	renderer.setBackgroundColor(0.903, 0.903, 0.903, 1.0);
	renderer.setEnvironmentGradientColor(0.903, 0.903, 0.903);
	renderer.enableTransparency();
	renderer.enableSampleShading();
	//renderer.disableShadows();
	renderer.disableFaceCulling();
	//renderer.applyGlobalTransform(glm::scale(glm::mat4(1), glm::vec3(0.001)));

	particleShader = Shader::create("particle", "assets/shaders/particle.vert", "assets/shaders/particle.frag");
	particleShader->setVec3("lightPos", glm::vec3(0, -200, 1000));

	binShader = Shader::create("bins", "assets/shaders/bin.vert", "assets/shaders/bin.frag");


	particleShader->use();
	particleShader->setInt("colorCycle", 4);
	binShader->use();
	binShader->setInt("colorCycle", 4);

	renderer.addShader(particleShader);
	renderer.addShader(binShader);

	Mesh_Ptr bbox = Primitives::createQuadCube(settings.bb.x, settings.bb.y, settings.bb.z);
	bbox->enableWireFrameMode();
	bbox->translate(glm::vec3(0, 0, settings.bb.z / 2.0));
	//scene.add(bbox);

	static_emitter = Primitives::createCylinder(5, 2, 10);
	static_emitter->enableWireFrameMode();
	static_emitter->rotate(glm::vec3(0, 120 * DEG_TO_RAD, 0));
	static_emitter->applyMeshTransform();
	static_emitter->translate(glm::vec3(-60, 0, 70));
	scene.add(static_emitter);

	pelton = ModelLoader::loadMesh("./assets/models/PELTON.stl");
	pelton->computeNormals();
	pelton->useFlatShading(true);
	pelton->centerMeshOrigin();
	pelton->setPosition(glm::vec3(0));
	pelton->rotate(glm::vec3(90 * DEG_TO_RAD, 0, 0));
	pelton->scale(0.05);
	pelton->applyMeshTransform();
	pelton->setMaterial("white plastic");
	pelton->translate(glm::vec3(0, 0, 50));


	float angle = 30;
	scene.add(pelton);
	pelton->computeBoundingBox();


	TransformObject_Ptr origin = TransformObject::create("origin", 30);
	//scene.add(origin);


}

void AppLayer::InitPhysics() {
	//Compute Shaders
	solver = StagedComputeShader::create("solver", "assets/shaders/solver/solver.comp", 7);
	prefixSum = StagedComputeShader::create("prefixSum", "assets/shaders/solver/prefix.sum.comp", 4);
	isoGen = ComputeShader::create("isoGen", "assets/shaders/solver/isoGen.comp");

	//create particle system
	ps = ParticleSystem::create("ParticleSystem", settings.max_pThread);
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
	isoGen->SetWorkgroupLayout(settings.iWkgCount);

	Console::info() << "Bin struct size :" << sizeof(Bin) << Console::endl;
	SSBO_Ptr<Bin> binBuffer = SSBO<Bin>::create("BinBuffer", settings.bThread);

	//attach Buffers
	ps->setShader(particleShader);
	ps->addProgram(solver);
	ps->addField<glm::vec4>("PositionBuffer");
	ps->addField<glm::vec4>("PredictedPositionBuffer");
	ps->addField<glm::vec4>("VelocityBuffer");
	ps->addField<float>("DensityBuffer");
	ps->addField<float>("LambdaBuffer");
	ps->addField<glm::uvec4>("MetaBuffer");
	ps->addField<CopyContent>("CopyBuffer");
	ps->addBuffer<glm::vec4>("EmitterPositionBuffer");
	ps->addBuffer(binBuffer);
	ps->solveLink(solver);
	ps->detach(solver);

	bs->setShader(binShader);
	bs->addProgram(prefixSum);
	bs->addField(binBuffer);
	bs->solveLink(prefixSum);
	bs->detach(prefixSum);

	ps->link(particleShader->name(), "PositionBuffer");
	ps->link(particleShader->name(), "PredictedPositionBuffer");
	ps->link(particleShader->name(), "VelocityBuffer");
	ps->link(particleShader->name(), "DensityBuffer");
	ps->link(particleShader->name(), "LambdaBuffer");
	ps->link(particleShader->name(), "MetaBuffer");
	ps->solveLink(particleShader);
	ps->detach(particleShader);

	ps->link(isoGen->name(), "PositionBuffer");
	ps->link(isoGen->name(), "VelocityBuffer");
	ps->link(isoGen->name(), "DensityBuffer");
	ps->link(isoGen->name(), "LambdaBuffer");
	ps->link(isoGen->name(), "MetaBuffer");
	ps->link(isoGen->name(), binBuffer->name());
	ps->solveLink(isoGen);
	ps->detach(isoGen);

	bs->link(binShader->name(), binBuffer->name());
	bs->solveLink(binShader);
	bs->detach(binShader);

	scene.add(ps);
	scene.add(bs);
	bs->hide();

	/*
	texture_debug = Texture2D::create(settings.volume_size.x, settings.volume_size.y, 1, 32);
	volume = Texture3D::create(settings.volume_size.x, settings.volume_size.y, settings.volume_size.z, 1, 32);
	volume->setUnit(0);
	isosurface = IsoSurface::create("isosurface", volume);

	Shared<PhongMaterial> water = createShared<PhongMaterial>("water");
	water->setAmbient(glm::vec3(0.204, 0.725, 0.98));
	water->setDiffuse(glm::vec3(0.737, 0.914, 1));
	water->setSpecular(glm::vec3(0.91, 0.969, 1));
	water->setShininess(0.8);
	isosurface->mesh()->setMaterial(water);

	//isosurface->mesh()->translate(settings.bb * glm::vec3(0, 0, 0.5));
	isosurface->mesh()->scale(settings.bb * glm::vec3(1.0));
	isosurface->hide();
	scene.add(isosurface);*/
}


void AppLayer::ResetSimulation() {
	elapsedTime = 0;
	lastSpawTime = 0;
	Console::info() << "Generating particles..." << Console::endl;

	BindingPointManager::instance().resetBindings();

	ps->detach(solver);
	bs->detach(prefixSum);
	ps->detach(particleShader);
	bs->detach(binShader);
	
	float spacing = settings.particleRadius * 3.0;
	auto cpu_position = std::vector<glm::vec4>();
	auto cpu_predictedPosition = std::vector<glm::vec4>();
	auto cpu_emitterPosition = std::vector<glm::vec4>();
	auto cpu_velocity = std::vector<glm::vec4>();
	auto cpu_density = std::vector<float>();
	auto cpu_lambda = std::vector<float>();
	auto cpu_meta = std::vector<glm::uvec4>();

	numParticles = 0;
	numEmitter = 0;

	/*
	{
		emitter->computeBoundingBox();
		emitter->voxelize(spacing);
		BoundingBox aabb = emitter->getBoundingBox();
		glm::vec3 bb_size = aabb.max - aabb.min;
		int gridSizeX = ceil(bb_size.x / spacing);
		int gridSizeY = ceil(bb_size.y / spacing);
		int gridSizeZ = ceil(bb_size.z / spacing);

		for (int i = 0; i < gridSizeX * gridSizeY * gridSizeZ; i++) {
			if (emitter->getVoxels()[i] != 0) {

				int index = i;
				int vz = index / (gridSizeX * gridSizeY);
				index -= (vz * gridSizeX * gridSizeY);
				int vy = index / gridSizeX;
				int vx = index % gridSizeX;

				float x = aabb.min.x + (vx + 0.5) * spacing;
				float y = aabb.min.y + (vy + 0.5) * spacing;
				float z = aabb.min.z + (vz + 0.5) * spacing;

				cpu_position.push_back(glm::vec4(x, y, z, 0.0));
				cpu_predictedPosition.push_back(glm::vec4(x, y, z, 0.0));
				cpu_velocity.push_back(glm::vec4(0));
				cpu_density.push_back(0.0);
				cpu_lambda.push_back(0.0);
				cpu_meta.push_back(glm::uvec4(FLUID, numParticles, numParticles, 0.0));
				numParticles++;
			}
		}
	}/**/

	/**/
	{
		static_emitter->computeBoundingBox();
		static_emitter->voxelize(spacing);
		BoundingBox aabb = static_emitter->getBoundingBox();
		glm::vec3 bb_size = aabb.max - aabb.min;
		int gridSizeX = ceil(bb_size.x / spacing);
		int gridSizeY = ceil(bb_size.y / spacing);
		int gridSizeZ = ceil(bb_size.z / spacing);

		for (int i = 0; i < gridSizeX * gridSizeY * gridSizeZ; i++) {
			if (static_emitter->getVoxels()[i] != 0) {

				int index = i;
				int vz = index / (gridSizeX * gridSizeY);
				index -= (vz * gridSizeX * gridSizeY);
				int vy = index / gridSizeX;
				int vx = index % gridSizeX;

				float x = aabb.min.x + (vx + 0.5) * spacing;
				float y = aabb.min.y + (vy + 0.5) * spacing;
				float z = aabb.min.z + (vz + 0.5) * spacing;

				cpu_emitterPosition.push_back(glm::vec4(x, y, z, 0.0));
				//cpu_meta.push_back(glm::uvec4(FLUID_EMITTER, numEmitter, numEmitter, 0.0));
				numEmitter++;
			}
		}
	}/**/


	{
		pelton->computeBoundingBox();
		pelton->voxelizeSurface(spacing, spacing);
		BoundingBox aabb = pelton->getBoundingBox();
		glm::vec3 bb_size = aabb.max - aabb.min;
		int gridSizeX = ceil(bb_size.x / spacing);
		int gridSizeY = ceil(bb_size.y / spacing);
		int gridSizeZ = ceil(bb_size.z / spacing);

		for (int i = 0; i < gridSizeX * gridSizeY * gridSizeZ; i++) {
			if (pelton->getVoxels()[i] != 0) {

				int index = i;
				int vz = index / (gridSizeX * gridSizeY);
				index -= (vz * gridSizeX * gridSizeY);
				int vy = index / gridSizeX;
				int vx = index % gridSizeX;

				float x = aabb.min.x + (vx + 0.5) * spacing;
				float y = aabb.min.y + (vy + 0.5) * spacing;
				float z = aabb.min.z + (vz + 0.5) * spacing;

				cpu_position.push_back(glm::vec4(x, y, z, 0.0));
				cpu_predictedPosition.push_back(glm::vec4(x, y, z, 0.0));
				cpu_velocity.push_back(glm::vec4(0));
				cpu_density.push_back(0.0);
				cpu_lambda.push_back(0.0);
				cpu_meta.push_back(glm::uvec4(BOUNDARY+1, numParticles, numParticles, 0.0));
				numParticles++;
			}
		}
	}

	while (cpu_position.size() < settings.max_pThread) {
		cpu_position.push_back(glm::vec4(0, 0, 0, 0.0));
		cpu_predictedPosition.push_back(glm::vec4(0, 0, 0, 0.0));
		cpu_velocity.push_back(glm::vec4(0));
		cpu_density.push_back(0.0);
		cpu_lambda.push_back(0.0);
		cpu_meta.push_back(glm::uvec4(FLUID_EMITTER, numParticles, numParticles, 0.0));
	}


	Console::info() << "Uploading buffer on device..." << Console::endl;

	settings.pThread = numParticles;
	settings.pWkgCount = (settings.pThread + settings.pWkgSize - 1) / settings.pWkgSize; //Total number of workgroup needed
	settings.blockSize = floor(log2f(settings.bThread));
	settings.blocks = (settings.bThread + settings.blockSize - 1) / settings.blockSize;
	settings.bWkgCount = (settings.blocks + settings.bWkgSize - 1) / settings.bWkgSize; //Total number of workgroup needed

	solver->SetWorkgroupLayout(settings.pWkgCount);
	prefixSum->SetWorkgroupLayout(settings.bWkgCount);

	
	ps->setInstancesCount(settings.max_pThread);
	bs->setInstancesCount(settings.bThread);
	ps->setActiveInstancesCount(settings.pThread);

	SyncUniforms();
	Console::info() << "Uploading buffer on device..." << Console::endl;
	ps->writeField("PositionBuffer", cpu_position);
	ps->writeField("PredictedPositionBuffer", cpu_predictedPosition);
	ps->writeField("VelocityBuffer", cpu_velocity);
	ps->writeField("DensityBuffer", cpu_density);
	ps->writeField("LambdaBuffer", cpu_lambda);
	ps->writeField("MetaBuffer", cpu_meta);
	ps->writeBuffer("EmitterPositionBuffer", cpu_emitterPosition);

	Console::success() << "Simulation ready" << Console::endl;
}


void AppLayer::SpawnParticle() {
	settings.pThread = numParticles + numEmitter;
	settings.pWkgCount = (settings.pThread + settings.pWkgSize - 1) / settings.pWkgSize; //Total number of workgroup needed
	solver->SetWorkgroupLayout(settings.pWkgCount);

	if (settings.pThread > 115000 && rot_speed < 50) rot_speed += ((50-rot_speed)/ 50)*0.3;


	ps->setActiveInstancesCount(settings.pThread);



	solver->execute(6);
	
	numParticles += numEmitter;
	solver->setUInt("numEmitter", 0);
	solver->setUInt("numParticles", numParticles);

	particleShader->use();
	particleShader->setUInt("numParticles", numParticles);

	isoGen->use();
	isoGen->setUInt("numParticles", numParticles);

	solver->use();
	solver->setUInt("numEmitter", numEmitter);
	solver->setUInt("numParticles", numParticles);
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

void AppLayer::Simulate(Merlin::Timestep ts) {
	elapsedTime += settings.timestep.value();

	solver->use();

	static double time = 0;
	GPU_PROFILE(nns_time,
		NeigborSearch();
	)
	
	GPU_PROFILE(solver_substep_time,
		for (int i = 0; i < settings.solver_substep; i++) {
			if (integrate) {
				time += settings.timestep.value() / settings.solver_substep;
				if (time - lastSpawTime > 0.0018) {
					SpawnParticle();
					lastSpawTime = time;
				}


				solver->execute(2);
				pelton->rotate(glm::vec3(0, -rot_speed * settings.timestep.value()/ settings.solver_substep, 0));

			}
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


	


}

void AppLayer::onImGuiRender() {
	//ImGui::ShowDemoWindow();
	ImGui::DockSpaceOverViewport((ImGuiViewport*)0, ImGuiDockNodeFlags_PassthruCentralNode);
	ImGui::Begin("Infos");


	ImGui::LabelText(std::to_string(numParticles).c_str(), "particles");
	ImGui::LabelText(std::to_string(settings.bThread).c_str(), "bins");
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
	if (ImGui::Checkbox("Particle shading", &transparency)) {
		if (transparency) ps->setDisplayMode(ParticleSystemDisplayMode::POINT_SPRITE_SHADED);
		else ps->setDisplayMode(ParticleSystemDisplayMode::POINT_SPRITE);
	}
	/*
	if (ImGui::Checkbox("Use IsoSurface", &use_isosurface)) {
		if (use_isosurface) isosurface->show();
		else isosurface->hide();
	}*/


	static bool showhelix = true;
	if (ImGui::Checkbox("Show helix", &showhelix)) {
		if (showhelix) pelton->show();
		else pelton->hide();
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


	ImGui::DragInt("Solver substep", &settings.solver_substep, 1, 1, 200);
	ImGui::DragInt("Solver iteration", &settings.solver_iteration, 1, 1, 200);

	if (ImGui::DragFloat3("Camera position", &model_matrix_translation.x, -10.0f, 10.0f)) {
		camera().setPosition(model_matrix_translation);
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
	if (ImGui::SliderFloat("Viscosity", &settings.artificialViscosityMultiplier.value(), 0.0, 200.0)) {
		solver->use();
		solver->setFloat("artificialViscosityMultiplier", settings.artificialViscosityMultiplier.value()*0.001);
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

	static bool first_frame = true;
	static int debug_layer = 0;

	/*ImGui::Begin("Debug");
	if (ImGui::SliderInt("Layer", (int*)&debug_layer, 0, settings.volume_size.z - 1) || first_frame)
	{
		// Copy one layer of the 3D volume texture into the "debug" 2D texture for display
		glCopyImageSubData(volume->id(), GL_TEXTURE_3D, 0, 0, 0, debug_layer,
			texture_debug->id(), GL_TEXTURE_2D, 0, 0, 0, 0,
			settings.volume_size.x, settings.volume_size.y, 1);

		first_frame = false;
	}
	ImGui::Image((void*)(intptr_t)texture_debug->id(), ImVec2(settings.volume_size.x, settings.volume_size.y), ImVec2(1, 1), ImVec2(0, 0));
	ImGui::End();*/

	if (ImGui::Button("Debug")) {
		throw("DEBUG");
		Console::info() << "DEBUG" << Console::endl;
	}
	ImGui::End();
}
