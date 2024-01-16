#include "ExampleLayer.h"

using namespace Merlin;
using namespace Merlin::Utils;
using namespace Merlin::Renderer;

#include <iostream>
#include <fstream>
#include <iomanip>

ExampleLayer::ExampleLayer() {
	Window* w = &Application::Get().GetWindow();
	_height = w->GetHeight();
	_width = w->GetWidth();
	camera = CreateShared<Camera>(_width, _height, Projection::Orthographic);
	camera->SetPosition(glm::vec3(0.235, 0.135, 1.0f));
	cameraController = CreateShared<CameraController2D>(camera);

	EnableGLDebugging();
	Console::SetLevel(ConsoleLevel::_INFO);

	// Init OpenGL settings
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
}



void ExampleLayer::CreateMesh() {
	Vertices vs;
	Vertex buf;
	float space = domainWidth / float(sqNodeCount);
	for (float y(0); y < sqNodeCount; y++) {
		for (float x(0); x < sqNodeCount; x++) {
			buf.position = glm::vec3(x*space, y * space, 0);
			vs.push_back(buf);
		}
	}

	Indices in;
	for (int y(0); y < sqNodeCount - 1; y++) {		//y
		for (int x(0); x < sqNodeCount - 1; x++) {	//x
			for (int k(0); k < 4; k++) {
				GLuint index = y * sqNodeCount + x; //First vertex of the quad
				in.push_back(index); //0
				in.push_back((y + 1) * sqNodeCount + x); //1
				in.push_back((y + 1) * sqNodeCount + x + 1); //2
				in.push_back(index + 1); //3
			}
		}
	}

	mesh = CreateShared<Primitive>(vs, in);
	mesh->SetDrawMode(GL_QUADS);
}

void ExampleLayer::LoadModels() {
	//Load models
	axis = Primitive::CreateCoordSystem();
	legend = Primitive::CreateRectangle(0.025, 0.5);
	legend->Translate(glm::vec3(0.05f/2.0f + 0.6, 0.5f/2.0f, 0.0f));

}

void ExampleLayer::LoadShaders() {
	//Shaders
	axisShader = CreateShared<Shader>("axis", "assets/shaders/axis.vert.glsl", "assets/shaders/axis.frag.glsl");
	legendShader = CreateShared<Shader>("legend", "assets/shaders/legend.vert.glsl", "assets/shaders/legend.frag.glsl");
	meshShader = CreateShared<Shader>("mesh", "assets/shaders/mesh.sph.vert.glsl", "assets/shaders/mesh.frag.glsl");

	//Compute Shaders
	init = CreateShared<ComputeShader>("init", "assets/shaders/heat.init.sph.glsl");
	physics = CreateShared<ComputeShader>("physics", "assets/shaders/heat.update.sph.glsl");
}

void ExampleLayer::InitShaders() {
	legendShader->Use();
	legendShader->SetUInt("colorCount", colorCount);

	meshShader->Use();
	meshShader->SetFloat("scale", 10);
	meshShader->SetUInt("colorCount", colorCount);

	init->Use();
	init->SetUInt("nodeCount", nodeCount);
	init->SetUInt("sqNodeCount", sqNodeCount);

	physics->Use();
	physics->SetUInt("nodeCount", nodeCount);
	physics->SetUInt("sqNodeCount", sqNodeCount);
	physics->SetDouble("dt", dt);
}


void ExampleLayer::SetColorGradient() {
	heatMap = CreateShared<SSBO>("ColorMapBuffer");
	heatMap->SetBindingPoint(2);

	std::vector<Color> colors;

	colors.push_back({ glm::vec3(0, 0, 1), 0.0f  });     // Blue.
	colors.push_back({ glm::vec3(0, 1, 1), 0.25f });     // Cyan.
	colors.push_back({ glm::vec3(0, 1, 0), 0.5f  });     // Green.
	colors.push_back({ glm::vec3(1, 1, 0), 0.75f });     // Yellow.
	colors.push_back({ glm::vec3(1, 0, 0), 1.0f  });     // Red.

	colorCount = colors.size();

	heatMap->Allocate<Color>(colors);
}


void ExampleLayer::CreateSolver() {
	//Create the buffer
	buffer = CreateShared<SSBO>("NodeBuffer");
	buffer->SetBindingPoint(1);

	std::vector<Node> nodes;
	for (size_t i(0); i < nodeCount; i++) {
		Node buf;
		buf.U = mesh->GetVertices()[i].position;
		buf.V = glm::vec3(0);
		buf.T = 20.0f;
		nodes.push_back(buf);
	}

	buffer->Allocate<Node>(nodes);
	Console::info("Application") << "Node allocated using " << long(buffer->size() / 1000) << "Ko" << Console::endl;

	solver = CreateShared<Solver>(nodeCount, 32);
	solver->AddComputeShader(init);
	solver->AddComputeShader(physics);
	solver->AddStorageBuffer(buffer);
}

void ExampleLayer::OnDetach() {}

void ExampleLayer::OnAttach(){
	sqNodeCount = 24;
	nodeCount = sqNodeCount * sqNodeCount; //2D Grid
	domainWidth = 0.05f;
	dt = 0.01;
	t = 0;

	CreateMesh();
	LoadModels();
	LoadShaders();
	SetColorGradient();
	InitShaders();
	CreateSolver();

	solver->Execute(0);
}

void ExampleLayer::OnUpdate(Timestep ts) {
	cameraController->OnUpdate(ts);
	updateFPS(ts);

	// Specify the color of the background
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


	if (!paused) {
		solver->Execute(1);
		t += dt;
	}
	buffer->Bind();
	buffer->Attach(meshShader);

	heatMap->Bind();
	heatMap->Attach(meshShader);
	heatMap->Attach(legendShader);

	meshShader->Use();
	meshShader->SetInt("mode", 1);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if(drawWiredMesh) mesh->Draw(meshShader, camera->GetViewProjectionMatrix());

	meshShader->Use();
	meshShader->SetInt("mode", 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if(drawMesh) mesh->Draw(meshShader, camera->GetViewProjectionMatrix());
	axis->Draw(axisShader, camera->GetViewProjectionMatrix());
	legend->Draw(legendShader, camera->GetViewProjectionMatrix());


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


void ExampleLayer::saveData() {
	std::vector<Node> result;
	result.resize(nodeCount);

	memcpy(result.data(), buffer->Map(), result.size() * sizeof(Node));

	std::ofstream file("file.float", std::ios::binary);
	for (int i = 0; i < result.size(); i++) {
		float num = result[i].T;
		file.write((char*)&num, sizeof(num));
	}
	file.close();
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
		if (ImGui::SmallButton("Pause simulation"))
			paused = !paused;
	}

	ImGui::Checkbox("Draw Mesh", &drawMesh);
	ImGui::Checkbox("Draw Wireframe", &drawWiredMesh);

	if (ImGui::SmallButton("Reset simulation")) {
		solver->Execute(0); //init position using init compute shader
		t = 0;
	}	
	
	if (ImGui::SmallButton("Save simulation")) {
		saveData();
	}

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
