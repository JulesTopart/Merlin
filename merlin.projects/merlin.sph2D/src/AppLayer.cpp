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
	camera->setFarPlane(1.0f);
	camera->setFOV(45.0f); //Use 90.0f as we are using cubemaps
	camera->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
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
	particleShader->Attach(*binBuffer);

	binShader->Use();
	binShader->Attach(*particleBuffer);
	binShader->Attach(*binBuffer);

	ResetSimulation();
}

void AppLayer::OnDetach(){}

void AppLayer::OnEvent(Event& event){
	camera->OnEvent(event);
}

float t = 0.0;

void AppLayer::OnUpdate(Timestep ts){

	t += ts;
	float x = light->position().x;
	float y = light->position().y;
	light->Translate(glm::vec3(cos(t) - x, sin(t) - y, 0.0));
	modelShader->Use();
	modelShader->SetVec3("lightPos", light->position());

	renderer.Clear();
	renderer.RenderScene(scene, *camera);
}

void AppLayer::OnImGuiRender()
{
	ImGui::Begin("Camera");

	model_matrix_translation = camera->GetPosition();
	if (ImGui::DragFloat3("Camera position", &model_matrix_translation.x, -100.0f, 100.0f)) {
		camera->SetPosition(model_matrix_translation);
	}
	ImGui::End();
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

	particleBuffer->Resize(settings.pThread);
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
	particleShader->SetUInt("colorCount", 5);
	particleShader->SetVec3("lightPos", glm::vec3(0, -200, 1000));

	binShader = Shader::Create("bins", "assets/shaders/bin.vert", "assets/shaders/bin.frag");
	binShader->noTexture();
	binShader->noMaterial();
	binShader->SetUInt("colorCount", 5);

	particleShader->Use();
	particleShader->SetInt("colorCycle", 0);
	binShader->Use();
	binShader->SetInt("colorCycle", 0);
	 
	renderer.AddShader(particleShader);
	renderer.AddShader(binShader);


	origin = TransformObject::Create("origin");
	scene.Add(origin);


	//scene.Add(Model::Create("cube",Primitives::CreateCube(20)));

}

void AppLayer::InitPhysics() {
	//Compute Shaders
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

	solver->SetWorkgroupLayout(settings.pWkgCount);
	prefixSum->SetWorkgroupLayout(settings.bWkgCount);

	//Allocate Buffers
	particleBuffer = SSBO<glm::vec4>::Create("ParticleBuffer");
	particleBuffer->Allocate(settings.pThread);

	SSBO_Ptr <glm::vec4> particleCpyBuffer = SSBO<glm::vec4>::Create("ParticleCpyBuffer");
	particleCpyBuffer->Allocate(settings.pThread);

	SSBO_Ptr <glm::vec2> velocityBuffer = SSBO<glm::vec2>::Create("VelocityBuffer");
	velocityBuffer->Allocate(settings.pThread);

	SSBO_Ptr <GLfloat> densityBuffer = SSBO<GLfloat>::Create("DensityBuffer");
	densityBuffer->Allocate(settings.pThread);


	binBuffer = SSBO<Bin>::Create("BinBuffer");
	binBuffer->Allocate(settings.bThread);

	//Attach Buffers
	particleSystem->AddComputeShader(solver);
	particleSystem->AddStorageBuffer(particleBuffer);
	particleSystem->AddStorageBuffer(velocityBuffer);
	particleSystem->AddStorageBuffer(densityBuffer);
	particleSystem->AddStorageBuffer(binBuffer);

	binSystem->AddComputeShader(prefixSum);
	binSystem->AddStorageBuffer(particleBuffer);
	binSystem->AddStorageBuffer(binBuffer);

	scene.Add(particleSystem);
	scene.Add(binSystem);
	//scene.Add(constraintSystem);
	binSystem->Hide();
}

void AppLayer::ResetSimulation() {

	settings.pThread = numParticles;
	particleSystem->SetInstancesCount(settings.pThread);

	elapsedTime = 0;
	Console::info() << "Generating particles..." << Console::endl;

	float spacing = settings.particleRadius * 2.0;
	auto& cpu_particles = particleBuffer->GetDeviceBuffer();

	glm::vec4 buf;

	buf.z = 1.0 / 1.0;//inverse mass
	buf.w = SOLID; // phase
	glm::vec3 cubeSize = glm::vec3(100, 10, 2);
	glm::ivec3 icubeSize = glm::vec3(cubeSize.x / spacing, cubeSize.y / spacing, cubeSize.y / spacing);

	for (int xi = 0; xi <= cubeSize.x / spacing; xi++)
		for (int yi = 0; yi <= cubeSize.y / spacing; yi++)
			for (int zi = 0; zi <= cubeSize.z / spacing; zi++) {
				float x = (xi * spacing) - (cubeSize.x / 2.0);
				float y = (yi * spacing) - (cubeSize.y / 2.0);
				float z = (zi * spacing) + 20;

				buf.x = x;
				buf.y = y;
				buf.z = z;
				cpu_particles.push_back(buf);
			}

	numParticles = cpu_particles.size();
	UpdateBufferSettings();

	particleBuffer->Bind();
	particleBuffer->Clear();
	particleBuffer->FreeHostMemory();
	particleBuffer->Upload();

	binBuffer->Bind();
	binBuffer->Clear();
	
	particleSystem->SetInstancesCount(numParticles);
	settings.pThread = numParticles;

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

	solver->SetInt("stabilizationPass", true);
	Simulate(0.016);
	solver->SetInt("stabilizationPass", false);
}


void AppLayer::NeigborSearch() {

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

	binBuffer->Bind();
	binBuffer->Clear(); //Reset neighbor search data

	solver->Execute(0); //Place particles in bins
	GPU_PROFILE(nns_time,
		NeigborSearch();
	)

	if (!paused) {
		elapsedTime += settings.timeStep;

		GPU_PROFILE(solver_substep_time,
			for (int i = 0; i < settings.solver_substep; i++) {
				solver->Execute(2); //Predict position
				for (int k = 0; k < settings.solver_iteration; k++) {
					solver->Execute(3); //Solve floor collision constraint
					solver->Execute(4); //Solve collision constraint
					//solver->Execute(5); //Solve distance constraint
				}
				solver->Execute(6); //Position delta
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



