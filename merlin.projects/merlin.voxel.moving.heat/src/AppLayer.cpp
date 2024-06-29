#include "Applayer.h"

#include <GLFW/glfw3.h>
#include <iostream>
#include <iomanip>

using namespace Merlin;

#define PROFILE(VAR, CODE) double start_ ## VAR ## _time = glfwGetTime(); CODE VAR = (glfwGetTime() - start_ ## VAR ## _time)*1000.0;
#define GPU_PROFILE(VAR, CODE) double start_ ## VAR ## _time = glfwGetTime(); CODE glFinish(); VAR = (glfwGetTime() - start_ ## VAR ## _time)*1000.0;

#define PROFILE_BEGIN(STARTVAR) STARTVAR = glfwGetTime();
#define PROFILE_END(STARTVAR, VAR) VAR = (glfwGetTime() - STARTVAR)*1000.0

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

	voxels->solveLink(binShader);

	PROFILE_END(total_start_time, total_time);
	PROFILE_BEGIN(total_start_time);

	GPU_PROFILE(render_time,
		renderer.clear();
		renderer.renderScene(scene, camera());
	)

	voxels->detach(binShader);


	voxels->solveLink(solver);

	if (!paused) {
		GPU_PROFILE(solver_total_time,
			Simulate(ts);
		)
	}

	voxels->detach(solver);
}



void AppLayer::SyncUniforms() {

	solver->use();

	solver->setUInt("numVoxels", numVoxels);

	domain->computeBoundingBox();
	BoundingBox aabb = domain->getBoundingBox();
	glm::vec3 bb_size = aabb.max - aabb.min;
	int gridSizeX = ceil(bb_size.x / (settings.particleRadius * 2.0));
	int gridSizeY = ceil(bb_size.y / (settings.particleRadius * 2.0));
	int gridSizeZ = ceil(bb_size.z / (settings.particleRadius * 2.0));

	solver->setUVec3("dim", glm::uvec3(gridSizeX, gridSizeY, gridSizeZ));
	solver->setFloat("dt", settings.timestep.value() / float(settings.solver_substep)); //Spawn particle after prediction

	binShader->use();
	binShader->setUInt("numVoxels", numVoxels);

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

	binShader = Shader::create("bins", "assets/shaders/bin.vert", "assets/shaders/bin.frag");
	binShader->noEnvironment();
	binShader->noMaterial();
	binShader->noTexture();
	binShader->noLights();
	binShader->noShadows();

	binShader->use();
	binShader->setInt("colorCycle", 2);

	renderer.addShader(binShader);

	Shared<Model> floor = ModelLoader::loadModel("./assets/models/bed.stl");
	floor->translate(glm::vec3(0.75, -0.25, -0.1));
	floor->scale(glm::vec3(1.025, 1.025, 1.0));
	floor->setMaterial("chrome");
	scene.add(floor);

	//modelShader->Use();
	//modelShader->setVec3("lightPos", glm::vec3(0.0, 10.0, 10));

	Shared<Model> floorSurface = Model::create("floorSurface", Primitives::createRectangle(316, 216));
	floorSurface->translate(glm::vec3(0.75, -0.25, 0));

	Shared<PhongMaterial> floorMat2 = createShared<PhongMaterial>("floorMat2");
	floorMat2->setAmbient(glm::vec3(0.02));
	floorMat2->setDiffuse(glm::vec3(0.95));
	floorMat2->setSpecular(glm::vec3(0.99));
	floorMat2->setShininess(0.7);
	floorMat2->loadTexture("assets/textures/bed.png", TextureType::DIFFUSE);

	floorSurface->setMaterial(floorMat2);
	scene.add(floorSurface);

	source = Primitives::createCylinder(0.2, 10, 30);
	domain = Primitives::createCube(100,10,1);
	domain->setMaterial("chrome");
	//domain->scale(4);
	//scene.add(domain);
	scene.add(source);

	scene.add(TransformObject::create("origin"));
}

void AppLayer::InitPhysics() {

	//Compute Shaders
	solver = StagedComputeShader::create("solver", "assets/shaders/solver/solver.comp", 2);

	//create particle system
	voxels = ParticleSystem::create("Voxels", settings.pThread);
	voxels->setShader(binShader);
	voxels->setMesh(Primitives::createQuadCube(settings.particleRadius * 2));
	voxels->setDisplayMode(ParticleSystemDisplayMode::MESH);

	solver->SetWorkgroupLayout(settings.pWkgCount);


	//attach Buffers
	voxels->setShader(binShader);
	voxels->addProgram(solver);
	voxels->addField<glm::vec4>("PositionBuffer");
	voxels->addField<float>("TemperatureBuffer");
	voxels->addField<float>("TemperatureOutBuffer");
	voxels->solveLink(solver);
	voxels->detach(solver);//test binding points

	voxels->link(binShader->name(), "PositionBuffer");
	voxels->link(binShader->name(), "TemperatureBuffer");
	voxels->solveLink(binShader);
	voxels->detach(binShader);//test binding points


	scene.add(voxels);
}


void AppLayer::ResetSimulation() {
	elapsedTime = 0;
	Console::info() << "Generating particles..." << Console::endl;

	float spacing = settings.particleRadius * 2.0;
	auto cpu_position = std::vector<glm::vec4>();
	auto cpu_temp = std::vector<float>();

	glm::vec3 cubeSize = glm::vec3(60, 195, 30);
	glm::ivec3 icubeSize = glm::vec3(cubeSize.x / spacing, cubeSize.y / spacing, cubeSize.z / spacing);
	numVoxels = 0;

	{
		domain->computeBoundingBox();
		domain->voxelize(spacing);
		BoundingBox aabb = domain->getBoundingBox();
		glm::vec3 bb_size = aabb.max - aabb.min;
		int gridSizeX = ceil(bb_size.x / spacing);
		int gridSizeY = ceil(bb_size.y / spacing);
		int gridSizeZ = ceil(bb_size.z / spacing);

		for (int i = 0; i < gridSizeX * gridSizeY * gridSizeZ; i++) {
			int index = i;
			int vz = index / (gridSizeX * gridSizeY);
			index -= (vz * gridSizeX * gridSizeY);
			int vy = index / gridSizeX;
			int vx = index % gridSizeX;

			float x = aabb.min.x + (vx + 0.5) * spacing;
			float y = aabb.min.y + (vy + 0.5) * spacing;
			float z = aabb.min.z + (vz + 0.5) * spacing;

			cpu_position.push_back(glm::vec4(x, y, z, -1)); //disable voxel
			cpu_temp.push_back(0); //ambient
			numVoxels++;
			
		}
	}



	Console::info() << "Uploading buffer on device..." << Console::endl;

	settings.pThread = numVoxels;
	settings.pWkgCount = (settings.pThread + settings.pWkgSize - 1) / settings.pWkgSize; //Total number of workgroup needed

	solver->SetWorkgroupLayout(settings.pWkgCount);

	voxels->setInstancesCount(settings.pThread);

	SyncUniforms();
	Console::info() << "Uploading buffer on device..." << Console::endl;
	voxels->writeField("PositionBuffer", cpu_position.data());
	voxels->writeField("TemperatureBuffer", cpu_temp.data());
}

void AppLayer::Simulate(Merlin::Timestep ts) {
	solver->use();
	solver->setFloat("dt", ts / float(settings.solver_substep)); // Spawn particle after prediction

	const double RECT_WIDTH = 100.0;
	const double RECT_HEIGHT = 10.0;
	const double SPEED = 50.0; // mm/s
	const double STEP_Y = 1.0;
	const double DT = ts / float(settings.solver_substep);

	static double x = -RECT_WIDTH / 2.0;
	static double y = STEP_Y/2.0 - RECT_HEIGHT / 2.0;
	static int direction = 1; // 1 for right, -1 for left

	source->setPosition(glm::vec3(x, y, 1));

	GPU_PROFILE(solver_substep_time,
		for (int i = 0; i < settings.solver_substep; i++) {
			x += SPEED * DT * direction;

			if (x > RECT_WIDTH / 2.0) {
				x = RECT_WIDTH / 2.0;
				direction = -1; // Change direction to left
				y += STEP_Y;
			}
			else if (x < -RECT_WIDTH / 2.0) {
				x = -RECT_WIDTH / 2.0;
				direction = 1; // Change direction to right
				y += STEP_Y;
			}

			// Ensure y stays within bounds
			if (y > RECT_HEIGHT / 2.0) {
				y = RECT_HEIGHT / 2.0;
			}

			solver->setVec3("heatPos", glm::vec3(x, y, 1));
			solver->execute(0);
			solver->execute(1);
		}
	)
		elapsedTime += ts;
}





void AppLayer::onImGuiRender() {
	//ImGui::ShowDemoWindow();
	ImGui::DockSpaceOverViewport((ImGuiViewport*)0, ImGuiDockNodeFlags_PassthruCentralNode);
	ImGui::Begin("Infos");


	ImGui::LabelText(std::to_string(numVoxels).c_str(), "particles");
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
		if (transparency) voxels->setDisplayMode(ParticleSystemDisplayMode::POINT_SPRITE_SHADED);
		else voxels->setDisplayMode(ParticleSystemDisplayMode::POINT_SPRITE);
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
	if (ImGui::Checkbox("Show voxels", &Pstate)) {
		if (Pstate) voxels->show();
		else voxels->hide();
	}

	ImGui::DragInt("Solver substep", &settings.solver_substep, 1, 1, 200);
	ImGui::DragInt("Solver iteration", &settings.solver_iteration, 1, 1, 200);

	if (ImGui::DragFloat3("Camera position", &model_matrix_translation.x, -100.0f, 100.0f)) {
		camera().setPosition(model_matrix_translation);
	}

	if (ImGui::InputFloat("Time step", &settings.timestep.value(), 0.0, 0.02f)) {
		solver->use();
		solver->setFloat("dt", settings.timestep.value());
	}

	static int colorMode = 2;
	static const char* options[] = { "Solid color", "Position" , "Temperature" };
	if (ImGui::ListBox("Colored field", &colorMode, options, 3)) {
		binShader->use();
		binShader->setInt("colorCycle", colorMode);
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
