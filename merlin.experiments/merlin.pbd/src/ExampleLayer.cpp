#include "ExampleLayer.h"
#include <GLFW/glfw3.h>

using namespace Merlin;
using namespace Merlin::Utils;
using namespace Merlin::Graphics;

#include <iostream>
#include <iomanip>
#include "Bunny.h"


#define PROFILE(VAR, CODE) double start_ ## VAR ## _time = glfwGetTime(); CODE VAR = (glfwGetTime() - start_ ## VAR ## _time)*1000.0;
#define GPU_PROFILE(VAR, CODE) double start_ ## VAR ## _time = glfwGetTime(); CODE glFinish(); VAR = (glfwGetTime() - start_ ## VAR ## _time)*1000.0;

#define PROFILE_BEGIN(STARTVAR) STARTVAR = glfwGetTime();
#define PROFILE_END(STARTVAR, VAR) VAR = (glfwGetTime() - STARTVAR)*1000.0

ExampleLayer::ExampleLayer() {
	Window* w = &Application::Get().GetWindow();
	int height = w->GetHeight();
	int width = w->GetWidth();
	camera = CreateShared<Camera>(width, height, Projection::Perspective);
	camera->setNearPlane(0.8f);
	camera->setFarPlane(800.0f);
	camera->setFOV(60); //Use 90.0f as we are using cubemaps
	camera->SetPosition(glm::vec3(0.0f, -140.0f, 40));
	camera->SetRotation(glm::vec3(0, 20, -270));
	cameraController = CreateShared<CameraController3D>(camera);
}

ExampleLayer::~ExampleLayer() {}

void ExampleLayer::OnAttach() {
	EnableGLDebugging();
	Window* wd = &Application::Get().GetWindow();
	_height = wd->GetHeight();
	_width = wd->GetWidth();
	ImGui::LoadIniSettingsFromDisk("imgui.ini");
	Console::SetLevel(ConsoleLevel::_INFO);

	InitGraphics();
	InitPhysics();
	SetColorGradient();
	particleShader->Use();
	particleShader->Attach(*particleBuffer, 0);
	particleShader->Attach(*binBuffer, 1);
	particleShader->Attach(*colorScaleBuffer, 2);
	particleShader->Attach(*heatMap, 3);

	binShader->Use();
	binShader->Attach(*particleBuffer, 0);
	binShader->Attach(*binBuffer, 1);
	binShader->Attach(*colorScaleBuffer, 2);
	binShader->Attach(*heatMap, 3);

	ResetSimulation();
}

void ExampleLayer::OnDetach() {
	
	
}

void ExampleLayer::OnEvent(Event& event) {
	camera->OnEvent(event);
	cameraController->OnEvent(event);
}

void ExampleLayer::OnUpdate(Timestep ts) {

	PROFILE_END(total_start_time, total_time); 
	PROFILE_BEGIN(total_start_time);
	
	cameraController->OnUpdate(ts);
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

void ExampleLayer::UpdateBufferSettings() {
	settings.pWkgCount = (settings.pThread + settings.pWkgSize - 1) / settings.pWkgSize; //Total number of workgroup needed
	settings.bWidth = max(settings.bx, max(settings.by, settings.bz)) / float(settings.bRes); //Width of a single bin in mm
	settings.bThread = int(settings.bx / (settings.bWidth)) * int(settings.by / (settings.bWidth)) * int(settings.bz / (settings.bWidth)); //Total number of bin (thread)
	settings.blockSize = floor(log2f(settings.bThread));
	settings.blocks = (settings.bThread + settings.blockSize - 1) / settings.blockSize;
	settings.bWkgCount = (settings.blocks + settings.bWkgSize - 1) / settings.bWkgSize; //Total number of workgroup needed

	init->SetWorkgroupLayout(settings.pWkgCount);
	solver->SetWorkgroupLayout(settings.pWkgCount);
	prefixSum->SetWorkgroupLayout(settings.bWkgCount);

	particleBuffer->Resize(settings.pThread);
	binBuffer->Resize(settings.bThread);
}

void ExampleLayer::InitGraphics() {
	// Init OpenGL stuff
	renderer.Initialize();
	renderer.SetBackgroundColor(0.203, 0.203, 0.203, 1.0);
	renderer.EnableTransparency();
	renderer.EnableSampleShading();


	//Shaders
	modelShader = Shader::Create("model", "assets/shaders/model.vert", "assets/shaders/model.frag");
	modelShader->SetVec3("lightPos", glm::vec3(0, 0, 100));

	particleShader = Shader::Create("particle", "assets/shaders/particle.vert", "assets/shaders/particle.frag");
	particleShader->noTexture();
	particleShader->noMaterial();
	particleShader->SetUInt("colorCount", 5);
	particleShader->SetVec3("lightPos", glm::vec3(0,-200,1000));

	binShader = Shader::Create("bins", "assets/shaders/bin.vert", "assets/shaders/bin.frag");
	binShader->noTexture();
	binShader->noMaterial();
	binShader->SetUInt("colorCount", 5);

	particleShader->Use();
	particleShader->SetInt("colorCycle", 0);
	binShader->Use();
	binShader->SetInt("colorCycle", 0);

	renderer.AddShader(modelShader);
	renderer.AddShader(particleShader);
	renderer.AddShader(binShader);




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
	floor->Rename("bed");
	floor->Translate(glm::vec3(0.75, -0.25, -0.05));
	floor->SetMaterial("chrome");
	floor->SetShader("model");
	scene.Add(floor);

	Shared<Model> floorSurface = Model::Create("floorSurface", Primitives::CreateRectangle(316, 216));
	floorSurface->Translate(glm::vec3(0.75, -0.25, 0));

	Shared<Material> floorMat2 = CreateShared<Material>("floorMat2");
	floorMat2->SetAmbient(glm::vec3(0.4));
	floorMat2->SetDiffuse(glm::vec3(0.9));
	floorMat2->SetSpecular(glm::vec3(0.2));
	floorMat2->SetShininess(0.8);
	floorMat2->LoadTexture("assets/textures/bed.png", TextureType::COLOR);

	floorSurface->SetMaterial(floorMat2);
	floorSurface->SetShader("model");
	scene.Add(floorSurface);

	//Box
	Shared<Model> box = Model::Create("box", Primitives::CreateQuadCube(settings.bx, settings.by, settings.bz));
	box->Translate(glm::vec3(0, 0, settings.bz / 2.0));
	box->SetMaterial("default");
	box->SetShader(modelShader);
	box->EnableWireFrameMode();
	scene.Add(box);


	origin = TransformObject::Create("origin");
	scene.Add(origin);


	//scene.Add(Model::Create("cube",Primitives::CreateCube(20)));

}

void ExampleLayer::InitPhysics() {
	//Compute Shaders
	init = CreateShared<ComputeShader>("init", "assets/shaders/solver/init.comp");
	solver = CreateShared<StagedComputeShader>("solver", "assets/shaders/solver/solver.comp", 6);
	prefixSum = CreateShared<StagedComputeShader>("prefixSum", "assets/shaders/solver/prefix.sum.comp", 4);

	//Create particle system
	particleSystem = deprecated_ParticleSystem::Create("ParticleSystem", settings.pThread);
	particleSystem->Translate(glm::vec3(0, 0, 0.5));

	//Define the mesh for instancing (Here a cube)
	//Shared<Mesh> particle = Primitives::CreateCube(0.5);
	//Shared<Mesh> particle = Primitives::CreateSphere(0.05*0.15, 10, 10);
	Shared<Mesh> particle = Primitives::CreatePoint();
	particle->Rename("particle");
	particle->SetShader(particleShader);
	particleSystem->SetMesh(particle);
	particleSystem->Translate(glm::vec3(0, 0, -0.5));
	particleSystem->SetDisplayMode(deprecated_ParticleSystemDisplayMode::POINT_SPRITE_SHADED);
	Console::info("Memory") << "size of FluidParticle is " << sizeof(FluidParticle) << Console::endl;

	Shared<Mesh> binInstance = Primitives::CreateQuadCube(settings.bWidth, false);
	binInstance->Rename("bin");
	binInstance->SetShader(binShader);
	binSystem = deprecated_ParticleSystem::Create("BinSystem", settings.bThread);
	binSystem->Translate(glm::vec3(0, 0, 0));
	binSystem->SetDisplayMode(deprecated_ParticleSystemDisplayMode::MESH);
	binSystem->SetMesh(binInstance);
	binSystem->EnableWireFrameMode();

	init->SetWorkgroupLayout(settings.pWkgCount);
	solver->SetWorkgroupLayout(settings.pWkgCount);
	prefixSum->SetWorkgroupLayout(settings.bWkgCount);
	//Create the buffer
	particleBuffer = SSBO<FluidParticle>::Create("ParticleBuffer");
	particleBuffer->Allocate(settings.pThread);	
	
	constraintBuffer = SSBO<DistanceContraint>::Create("ContraintBuffer");
	constraintBuffer->Allocate(32* settings.pThread);

	binBuffer = SSBO<Bin>::Create("BinBuffer");
	binBuffer->Allocate(settings.bThread);


	colorScaleBuffer = SSBO<ColorScale>::Create("ColorScaleBuffer");
	std::vector<ColorScale> scales;
	scales.push_back({ 0,1 });
	scales.push_back({ 0,100 });
	scales.push_back({ 0,400 });
	scales.push_back({ 0,50 });
	colorScaleBuffer->LoadData(scales);

	particleSystem->AddComputeShader(init);
	particleSystem->AddComputeShader(solver);
	particleSystem->AddStorageBuffer(particleBuffer);
	particleSystem->AddStorageBuffer(binBuffer);
	particleSystem->AddStorageBuffer(colorScaleBuffer);

	binSystem->AddComputeShader(prefixSum);
	binSystem->AddStorageBuffer(particleBuffer);
	binSystem->AddStorageBuffer(binBuffer);
	binSystem->AddStorageBuffer(colorScaleBuffer);

	scene.Add(particleSystem);
	scene.Add(binSystem);
	//scene.Add(constraintSystem);
	binSystem->Hide();
}

void ExampleLayer::SetColorGradient() {
	std::vector<glm::vec4> colors;
	colors.push_back(glm::vec4(0, 0, 1, 0.0f));     // Blue.
	colors.push_back(glm::vec4(0, 1, 1, 0.25f));     // Cyan.
	colors.push_back(glm::vec4(0, 1, 0, 0.5f));     // Green.
	colors.push_back(glm::vec4(1, 1, 0, 0.75f));     // Yellow.
	colors.push_back(glm::vec4(1, 0, 0, 1.0f));     // Red.
	colorCount = colors.size();

	heatMap = SSBO<glm::vec4>::Create("ColorMapBuffer");
	heatMap->LoadData(colors);
}

void ExampleLayer::ResetSimulation() {

	settings.pThread = numParticles;
	particleSystem->SetInstancesCount(settings.pThread);

	particleBuffer->Bind();
	particleBuffer->Clear();
	particleBuffer->FreeHostMemory();



	elapsedTime = 0;
	Console::info() << "Generating particles..." << Console::endl;


	float spacing = settings.particleRadius*2.0;

	std::vector<glm::vec3> bunny = GenerateVoxelBunny(spacing);
	auto& cpu_particles = particleBuffer->GetDeviceBuffer();

	FluidParticle buf;
	buf.acceleration[0] = 0;
	buf.acceleration[1] = 0;
	buf.acceleration[2] = 0;
	buf.velocity[0] = 0;
	buf.velocity[1] = 0;
	buf.velocity[2] = 0;
	//buf.density = 0;
	buf.temperature = 273.15;//ambient
	buf.binIndex = 0;
	buf.newIndex = 0;

	/*
	buf.phase = SOLID; //Fluid body
	const float height = 80;
	const float goldenRatio = (1.0 + sqrt(5.0)) / 2.0;
	const float angleIncrement = 3.1415926 * 2.0 * goldenRatio;
	const int radius = 8;
	for (float r = spacing; r < radius; r += spacing)
		for (int i = 0; i < 4 * 3.1415926 * r * r; ++i) {
			float t = float(i) / float(4 * 3.1415926 * r * r);
			float inclinationAngle = acos(1 - 2 * t);  // polar angle
			float azimuthalAngle = angleIncrement * i;  // azimuthal angle
			buf.position[0] = r * sin(inclinationAngle) * cos(azimuthalAngle);
			buf.position[1] = r * sin(inclinationAngle) * sin(azimuthalAngle);
			buf.position[2] = r * cos(inclinationAngle) + height;

			buf.initial_position = buf.position;
			cpu_particles.push_back(buf);
		}
	*/
	
	/*
	buf.phase = SOLID; //Rigid bunny
	for (auto& v : bunny) {
		buf.position[0] = v.x;
		buf.position[1] = v.y;
		buf.position[2] = v.z + 10;
		buf.initial_position = buf.position;
		cpu_particles.push_back(buf);
	}*/
	

	/*
	buf.phase = SOLID; //Rigid cube
	glm::vec3 cubeSize = glm::vec3(20, 10, 10);
	glm::uvec3 icubeSize = glm::vec3(cubeSize.x/spacing, cubeSize.y / spacing, cubeSize.y / spacing);

	for (int xi = 0; xi < cubeSize.x / spacing; xi++)
	for (int yi = 0; yi < cubeSize.y / spacing; yi++)
	for (int zi = 0; zi < cubeSize.z / spacing; zi++) {
		float x = (xi*spacing) - (cubeSize.x / 2.0);
		float y = (yi*spacing) - (cubeSize.y / 2.0);
		float z = (zi*spacing)+ 5;
	
		buf.position[0] = x;
		buf.position[1] = y;
		buf.position[2] = z;
		buf.initial_position = buf.position;
		cpu_particles.push_back(buf);
	}*/

	/*
	buf.phase = SOLID; //Rigid beam
	glm::vec3 cubeSize = glm::vec3(100, 10, 2);
	glm::ivec3 icubeSize = glm::vec3(cubeSize.x / spacing, cubeSize.y / spacing, cubeSize.y / spacing);

	for (int xi = 0; xi <= cubeSize.x / spacing; xi++)
		for (int yi = 0; yi <= cubeSize.y / spacing; yi++)
			for (int zi = 0; zi <= cubeSize.z / spacing; zi++) {
				float x = (xi * spacing) - (cubeSize.x / 2.0);
				float y = (yi * spacing) - (cubeSize.y / 2.0);
				float z = (zi * spacing) + 20;

				buf.position[0] = x;
				buf.position[1] = y;
				buf.position[2] = z;
				buf.initial_position = buf.new_position = buf.pposition = buf.position;
				cpu_particles.push_back(buf);
			}
	*/

	
	buf.temperature = 273.15 + 200;//ambient
	buf.phase = SOLID; //Rigid beam
	glm::vec3 cubeSize = glm::vec3(1, 1, 50);
	glm::ivec3 icubeSize = glm::vec3(cubeSize.x / spacing, cubeSize.y / spacing, cubeSize.z / spacing);
	buf.temperature = 273.15 + 215;//ambient
	for (int zi = 0; zi <= cubeSize.z / spacing; zi++)
		for (int yi = 0; yi <= cubeSize.y / spacing; yi++)
			for (int xi = 0; xi <= cubeSize.x / spacing; xi++) {
				float x = (xi * spacing) - (cubeSize.x / 2.0);
				float y = (yi * spacing) - (cubeSize.y / 2.0);
				float z = (zi * spacing) + 20;

				buf.position[0] = x;
				buf.position[1] = y;
				buf.position[2] = z;
				buf.initial_position = buf.new_position = buf.pposition = buf.position;
				cpu_particles.push_back(buf);
			}
			

	
	buf.temperature = 273.15 + 60;//ambient
	buf.phase = BOUNDARY; //Boundaries body
	for (float x = -settings.bx / 2.0; x < settings.bx / 2.0; x += spacing) {
		for (float y = -settings.by / 2.0; y < settings.by / 2.0; y += spacing) {
			buf.position[0] = x;
			buf.position[1] = y;
			buf.position[2] = 0;
			cpu_particles.push_back(buf);
			buf.position[2] = settings.bz;
			//cpu_particles.push_back(buf);
		}
	}/*
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
	}*/

	
	particleBuffer->Upload();
	Console::info() << "Loaded Stanford rabbit and a sphere in particle buffer (" << cpu_particles.size() << " particles )" << Console::endl;

	binBuffer->Bind();
	binBuffer->Clear();
	numParticles = cpu_particles.size();
	particleSystem->SetInstancesCount(numParticles);

	settings.pThread = numParticles;

	particleShader->Use();
	particleShader->SetUInt("numParticles", numParticles);
	particleShader->SetFloat("smoothingRadius", settings.H); // Kernel radius // 5mm
	particleShader->SetFloat("particleRadius", settings.particleRadius); // visual radius // 5mm
	particleShader->SetFloat("particleMass", settings.particleMass); // Kernel radius // 5mm
	particleShader->SetFloat("REST_DENSITY", settings.REST_DENSITY); // Kernel radius // 5mm

	UpdateBufferSettings();


	solver->Use();
	solver->SetUInt("numParticles", numParticles);
	solver->SetFloat("smoothingRadius", settings.H); // Kernel radius // 5mm
	solver->SetFloat("particleRadius", settings.particleRadius); // visual radius // 5mm
	solver->SetFloat("particleMass", settings.particleMass); // Kernel radius // 5mm
	solver->SetFloat("REST_DENSITY", settings.REST_DENSITY); // Kernel radius // 5mm
	solver->SetInt("stabilizationPass", true);
	Simulate(0.016);
	solver->SetInt("stabilizationPass", false);
}


glm::uvec3 ExampleLayer::getBinCoord(glm::vec3 position) {
	position += glm::vec3(150, 100, 125);
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
	index -= (z * (settings.bx / (settings.bWidth)) * (settings.by / (settings.bWidth)));
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
	solver->SetUInt("numParticles", numParticles); //Spawn particle after prediction
	solver->SetFloat("dt", settings.timeStep / float(settings.solver_substep)); //Spawn particle after prediction

	binBuffer->Bind();
	binBuffer->Clear(); //Reset neighbor search data


	solver->Execute(0); //Place particles in bins
	GPU_PROFILE(nns_time,
		NeigborSearch();
		solver->Use();
		solver->Execute(1); //Sort
	)

	solver->Use();

	if (!paused) {
		elapsedTime += settings.timeStep;

		colorScaleBuffer->Bind();
		colorScaleBuffer->Clear();
		
		GPU_PROFILE(solver_substep_time,
		for (int i = 0; i < settings.solver_substep; i++) {
			solver->Execute(2); //Predict position
			for (int k = 0; k < settings.solver_iteration; k++) {
				solver->Execute(3); //Solve floor collision constraint
				solver->Execute(4); //Solve collision constraint
				solver->Execute(5); //Solve distance constraint
			}
			solver->Execute(6); //Position delta
		})
	}
}

void ExampleLayer::updateFPS(Timestep ts) {
	if (FPS_sample == 0) {
		FPS = ts;
	}
	else {
		FPS += ts;
	}
	FPS_sample++;
}



static glm::vec4 DeltaTimeToColor(float dt)
{
	constexpr glm::vec3 colors[] = {
		{0.f, 0.f, 1.f}, // blue
		{0.f, 1.f, 0.f}, // green
		{1.f, 1.f, 0.f}, // yellow
		{1.f, 0.f, 0.f}, // red
	};
	constexpr float dts[] = {
		1.f / 120.f,
		1.f / 60.f,
		1.f / 30.f,
		1.f / 15.f,
	};
	if (dt < dts[0])
		return glm::vec4(colors[0], 1.f);
	for (size_t i = 1; i < _countof(dts); ++i)
	{
		if (dt < dts[i])
		{
			const float t = (dt - dts[i - 1]) / (dts[i] - dts[i - 1]);
			return glm::vec4(glm::mix(colors[i - 1], colors[i], t), 1.f);
		}
	}
	return glm::vec4(colors[_countof(dts) - 1], 1.f);
}



void ExampleLayer::OnImGuiRender()
{

	ImGui::ShowDemoWindow();
	ImGui::DockSpaceOverViewport((ImGuiViewport*)0, ImGuiDockNodeFlags_PassthruCentralNode);
	ImGui::Begin("Infos");

	model_matrix_translation = camera->GetPosition();
	camera_speed = cameraController->GetCameraSpeed();

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

	if (ImGui::SliderFloat("Camera speed", &camera_speed, 0.0, 50.0f)) {
		cameraController->SetCameraSpeed(camera_speed);
	}
	if (ImGui::SliderFloat("Simulation speed", &sim_speed, 0.0, 5.0f)) {
		solver->Use();
		solver->SetFloat("speed", sim_speed);
	}

	if (ImGui::SliderFloat("Smoothing radius", &settings.H, 1*settings.particleRadius, 3*settings.particleRadius)) {
		solver->Use();
		solver->SetFloat("smoothingRadius", settings.H); // Kernel radius // 5mm
		particleShader->Use();
		particleShader->SetFloat("smoothingRadius", settings.H); // Kernel radius // 5mm
	}
	if (ImGui::SliderFloat("particle radius", &settings.particleRadius, 0.1, 3.0)) {
		solver->Use();
		solver->SetFloat("particleRadius", settings.particleRadius);
		particleShader->Use();
		particleShader->SetFloat("particleRadius", settings.particleRadius); // Kernel radius // 5mm
	}

	if (ImGui::SliderFloat("Rest density", &settings.REST_DENSITY, 0.1, 2.0)) {
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

	if (ImGui::SliderFloat("Fluid particle mass", &settings.particleMass, 0.0001, 2.0)) {
		solver->Use();
		solver->SetFloat("particleMass", settings.particleMass);
	}

	static int colorMode = 0;
	static const char* options[] = { "Solid color", "Bin index", "Density", "Temperature", "Lambda", "Mass", "Neighbors" };
	if (ImGui::ListBox("Colored field", &colorMode, options, 7)) {
		particleShader->Use();
		particleShader->SetInt("colorCycle", colorMode);
		binShader->Use();
		binShader->SetInt("colorCycle", colorMode);
	}

	static int particleTest = 2000;
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
		colorScaleBuffer->Bind();
		colorScaleBuffer->Download();

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
