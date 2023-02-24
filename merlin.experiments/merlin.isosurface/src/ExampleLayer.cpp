#include "ExampleLayer.h"

using namespace Merlin;
using namespace Merlin::Utils;
using namespace Merlin::Renderer;

#include <iostream>
#include <iomanip>

ExampleLayer::ExampleLayer() {
	Window* w = &Application::Get().GetWindow();
	_height = w->GetHeight();
	_width = w->GetWidth();
	camera = CreateShared<Camera>(_width, _height, Projection::Perspective);
	camera->SetPosition(glm::vec3(-2.0f, 0.0f, 0.6f));
	camera->Rotate(glm::vec3(0,0,-1.55));
	cameraController = CreateShared<CameraController3D>(camera);

	EnableGLDebugging();
	Console::SetLevel(ConsoleLevel::_INFO);

	// Init OpenGL settings

	if (true) { //enable transparency
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
	}

	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
}



void ExampleLayer::CreateMesh() {
	//voxels = Primitive::CreateSphere(0.040 / sqNodeCount /2, 8, 8);
	voxels = Primitive::CreateCube(0.050 * 1.0 / sqNodeCount);
	voxels->Translate(glm::vec3(-0.475f, -0.475f, 0));
}

void ExampleLayer::LoadModels() {
	//Load models
	axis = Primitive::CreateCoordSystem();
}

void ExampleLayer::LoadShaders() {
	//Shaders
	axisShader = CreateShared<Shader>("axis", "assets/shaders/axis.vert.glsl", "assets/shaders/axis.frag.glsl");
	voxelShader = CreateShared<Shader>("voxel", "assets/shaders/voxel.vert.glsl", "assets/shaders/voxel.frag.glsl");
	isoSurfaceShader = CreateShared<Shader>("isosurface", "assets/shaders/isosurface.vert.glsl", "assets/shaders/isosurface.frag.glsl");
	//Compute Shaders
	physics = CreateShared<ComputeShader>("physics", "assets/tensor/noise.glsl");
	isosurfaceGen = CreateShared<ComputeShader>("isosurface", "assets/tensor/isosurface.glsl");
}

void ExampleLayer::InitShaders() {

	voxelShader->Use();
	voxelShader->SetFloat("scale", 20);

	physics->Use();
	physics->SetUInt("nodeCount", nodeCount);
	physics->SetUInt("sqNodeCount", sqNodeCount);
	physics->SetDouble("dt", dt);

	
	isosurfaceGen->Use();

	isosurfaceGen->SetUInt("chunk_size", isoSurfaceResolution);
	isosurfaceGen->SetUInt("chunk_size2", isoSurfaceResolution * isoSurfaceResolution);
	
}


void ExampleLayer::CreateSolver() {
	//Create the buffer
	buffer = CreateShared<SSBO>("NodeBuffer");
	buffer->SetBindingPoint(1);

	std::vector<Node> nodes;
	{
		Console::print() << "Generating nodes..." << Console::endl;
		Node buf;
		float space = domainWidth / float(sqNodeCount);
		for (size_t k(0); k < sqNodeCount; k++) {
			for (size_t j(0); j < sqNodeCount; j++) {
				for (size_t i(0); i < sqNodeCount; i++) {
					buf.U = glm::vec3(space * float(i), space * float(j), space * float(k));
					buf.V = glm::vec3(0);
					buf.enable = 0;
					buf.index = k * sqNodeCount * sqNodeCount + j * sqNodeCount + i;
					nodes.push_back(buf);
				}
			}
			if (k % (nodeCount / 100) == 0) Console::printProgress(float(float(k) / float(nodeCount)));
		}
		Console::printProgress(1.0f);
		Console::print() << Console::endl;
	}


	buffer->Allocate<Node>(nodes);
	Console::info("Application") << "Node allocated using " << long(buffer->size() / 1000) << "Ko" << Console::endl;


	isobuffer = CreateShared<SSBO>("GeomBuffer");
	isobuffer->SetBindingPoint(2);


	GLsizei isoVertexCount = pow((isoSurfaceResolution * 15), 3); //Worst case 15 vertex per voxel in 3D

	std::vector<IsoVertex> isoVertices;
	isoVertices.resize(isoVertexCount);
	isobuffer->Allocate<IsoVertex>(isoVertices);

	std::vector<Vertex> vertices;
	vertices.resize(isoVertexCount);
	isoSurface = CreateShared<Primitive>(vertices);
	isoSurface->Translate(glm::vec3(-0.475f, -0.475f, 0));
	

	physics->PrintLimits();
	solver = CreateShared<Solver>(nodeCount, 64);
	solver->AddComputeShader(physics);
	solver->AddStorageBuffer(buffer);
	solver->AddStorageBuffer(isobuffer);

}

void ExampleLayer::OnDetach() {}

void ExampleLayer::OnAttach(){
	sqNodeCount = 32;
	isoSurfaceResolution = 32;
	nodeCount = sqNodeCount * sqNodeCount * sqNodeCount; //3D Grid
	domainWidth = 0.05f;
	dt = 0.003;
	t = 0;

	CreateMesh();
	LoadModels();
	LoadShaders();
	InitShaders();
	CreateSolver();
}

void ExampleLayer::OnUpdate(Timestep ts) {
	cameraController->OnUpdate(ts);
	updateFPS(ts);

	// Specify the color of the background
	glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glLineWidth(5);
	axis->Draw(axisShader, camera->GetViewProjectionMatrix());
	glLineWidth(1);

	simulate();
	if (!paused) {
		solver->Execute(physics);
	}

	if (drawMesh) voxels->DrawInstanced(voxelShader, camera->GetViewProjectionMatrix(), nodeCount);
	else isoSurface->Draw(isoSurfaceShader, camera->GetViewProjectionMatrix());

}

void ExampleLayer::simulate(){

	if (!paused) {
		physics->Use();
		physics->SetFloat("iTime", t);
		t += float(dt);
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

void ExampleLayer::OnEvent(Event& event) {
	camera->OnEvent(event);
	cameraController->OnEvent(event);
}

void ExampleLayer::OnImGuiRender()
{
	ImGui::Begin("Infos");

	model_matrix_translation = camera->GetPosition();
	camera_speed = cameraController->GetCameraSpeed();


	ImGui::LabelText("t=", std::to_string(t).c_str());

	if (FPS_sample > 0) {
		ImGui::LabelText("FPS", std::to_string(1.0f / (FPS / FPS_sample)).c_str());
		if (FPS_sample > 50) FPS_sample = 0;
	}

	if (paused) {
		if (ImGui::ArrowButton("Run simulation", 1))
			paused = !paused;
	}
	else {
		if (ImGui::SmallButton("Pause simulation")) {
			paused = !paused;
			solver->SetThread(1);
			solver->Execute(isosurfaceGen);
			solver->SetThread(64);
		}

	}

	

	ImGui::Checkbox("Draw Mesh", &drawMesh);
	ImGui::Checkbox("Draw Wireframe", &drawWiredMesh);

	if (ImGui::DragFloat3("Camera position", &model_matrix_translation.x, -100.0f, 100.0f)) {
		camera->SetPosition(model_matrix_translation);
	}
	if (ImGui::SliderFloat("Camera speed", &camera_speed, 0.0, 100.0f)) {
		cameraController->SetCameraSpeed(camera_speed);
	}
	if (ImGui::SliderFloat("Simulation speed", &sim_speed, 0.0, 20.0f)) {
	}

	ImGui::End();
}
