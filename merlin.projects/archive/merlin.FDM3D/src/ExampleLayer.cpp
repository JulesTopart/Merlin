#include "ExampleLayer.h"

using namespace Merlin;
using namespace Merlin::Utils;
using namespace Merlin::Graphics;

#include <iostream>
#include <iomanip>

ExampleLayer::ExampleLayer() {
	Window* w = &Application::Get().GetWindow();
	_height = w->GetHeight();
	_width = w->GetWidth();
	camera = CreateShared<Camera>(_width, _height, Projection::Perspective);
	camera->SetPosition(glm::vec3(-2.0f, 0.0f, 1.0f));
	cameraController = CreateShared<CameraController3D>(camera);

	EnableGLDebugging();
	Console::SetLevel(ConsoleLevel::_INFO);

	// Init OpenGL settings
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
}



void ExampleLayer::CreateMesh() {
	//voxels = Primitive::CreateSphere(0.040 / sqNodeCount /2, 8, 8);
	voxels = Primitives::CreateCube(0.050 * 0.8 / sqNodeCount);
	voxels->Translate(glm::vec3(-0.475f, -0.475f, 0));
}

void ExampleLayer::LoadModels() {
	//Load models
	axis = Primitive::CreateCoordSystem();
	bed = Primitive::CreateRectangle(1.0f, 1.0f);
	bed->Translate(glm::vec3(0.0f, 0.0f, -0.01f));
}

void ExampleLayer::LoadShaders() {
	//Shaders
	axisShader = CreateShared<Shader>("axis", "assets/shaders/axis.vert.glsl", "assets/shaders/axis.frag.glsl");
	modelShader = CreateShared<Shader>("model", "assets/shaders/model.vert.glsl", "assets/shaders/model.frag.glsl");
	voxelShader = CreateShared<Shader>("voxel", "assets/shaders/voxel.vert.glsl", "assets/shaders/voxel.frag.glsl");

	//Compute Shaders
	init = CreateShared<ComputeShader>("init", "assets/physics/heat.init.glsl");
	physics = CreateShared<ComputeShader>("physics", "assets/physics/heat.update.glsl");
}

void ExampleLayer::InitShaders() {

	voxelShader->Use();
	voxelShader->SetUInt("colorCount", colorCount);
	voxelShader->SetFloat("scale", 20);

	init->Use();
	init->SetUInt("nodeCount", nodeCount);
	init->SetUInt("sqNodeCount", sqNodeCount);

	physics->Use();
	physics->SetUInt("nodeCount", nodeCount);
	physics->SetUInt("sqNodeCount", sqNodeCount);
	physics->SetUInt("sqBinCount", hashResolution);
	physics->SetDouble("dt", dt);

	modelShader->Use();
	modelShader->SetUniform3f("lightPos", glm::vec3(0, 2, 2));
	modelShader->SetUniform3f("lightColor", glm::vec3(0.3, 0.3, 0.3));
	modelShader->SetUniform3f("viewPos", camera->GetPosition());
	modelShader->SetFloat("shininess", 16.0f);
}


void ExampleLayer::SetColorGradient() {


	std::vector<Color> colors;
	colors.push_back({ glm::vec3(0, 0, 1), 0.0f  });     // Blue.
	colors.push_back({ glm::vec3(0, 1, 1), 0.25f });     // Cyan.
	colors.push_back({ glm::vec3(0, 1, 0), 0.5f  });     // Green.
	colors.push_back({ glm::vec3(1, 1, 0), 0.75f });     // Yellow.
	colors.push_back({ glm::vec3(1, 0, 0), 1.0f  });     // Red.
	colorCount = colors.size();

	heatMap = CreateShared<SSBO>("ColorMapBuffer");
	heatMap->SetBindingPoint(0);
	heatMap->Allocate<Color>(colors);
}


GLsizeiptr hash(glm::vec3 p, int res) {
	GLsizeiptr ix = (p.x / 0.05f) * float(res);
	GLsizeiptr iy = (p.y / 0.05f) * float(res);
	GLsizeiptr iz = (p.z / 0.05f) * float(res);

	//return ix * 92837111 ^ iy * 689287499 ^ iz * 283923481;
	return ix + iy * res + iz * res * res;
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
					buf.T = 20.0f;
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
	
	std::vector<std::pair<GLsizeiptr, GLsizeiptr>> hashlist;
	{
		Console::print() << "Generating hash..." << Console::endl;
		for (GLsizeiptr i(0); i < nodes.size(); i++) {
			//Compute spatial hash
			hashlist.push_back(std::make_pair(hash(nodes[i].U, hashResolution), i));
			if (i % (nodes.size() / 100) == 0) Console::printProgress(float(float(i) / float(nodes.size())));
		}
		Console::printProgress(1.0f);
		Console::print() << Console::endl;
		std::sort(hashlist.begin(), hashlist.end()); //Sort list
	}

	std::vector<Bin> bins;
	std::vector<Node> sorted_nodes;
	{
		Console::print() << "Sorting nodes..." << Console::endl;
		GLsizeiptr hash = -1;
		for (GLsizeiptr i(0); i < hashlist.size(); i++) {
			if (hash != hashlist[i].first) {
				if (bins.size() > 0) bins[bins.size() - 1].end = i - 1;
				bins.emplace_back();
				bins[bins.size() - 1].start = i;
				hash = hashlist[i].first;
			}
			sorted_nodes.push_back(nodes[hashlist[i].second]);

			if (i % (hashlist.size() / 100) == 0) Console::printProgress(float(float(i) / float(hashlist.size())));
		};
		bins[bins.size() - 1].end = hashlist.size() - 1;

		Console::printProgress(1.0f);
		Console::print() << Console::endl;
	}


	buffer->Allocate<Node>(sorted_nodes);
	Console::info("Application") << "Node allocated using " << long(buffer->size() / 1000) << "Ko" << Console::endl;

	binBuffer = CreateShared<SSBO>("BinBuffer");
	binBuffer->SetBindingPoint(2);
	binBuffer->Allocate<Bin>(bins);
	Console::info("Application") << "Bins allocated using " << long(binBuffer->size() / 1000) << "Ko" << Console::endl;

	physics->PrintLimits();

	solver = CreateShared<Solver>(nodeCount, 64);
	solver->AddComputeShader(init);
	solver->AddComputeShader(physics);
	solver->AddStorageBuffer(buffer);
	solver->AddStorageBuffer(binBuffer);
}

void ExampleLayer::OnDetach() {}

void ExampleLayer::OnAttach(){
	sqNodeCount = 128;
	hashResolution = 100;
	nodeCount = sqNodeCount * sqNodeCount * sqNodeCount; //3D Grid
	domainWidth = 0.05f;
	dt = 0.001;
	t = 0;

	speed = 5*(domainWidth) / sqNodeCount;
	u = glm::vec3(0, 0, 0);
	v = glm::vec3(speed, 0, 0);

	CreateMesh();
	LoadModels();
	LoadShaders();
	SetColorGradient();
	InitShaders();
	CreateSolver();

	solver->Execute(init);
}

void ExampleLayer::OnUpdate(Timestep ts) {
	cameraController->OnUpdate(ts);
	updateFPS(ts);

	// Specify the color of the background
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//simulate();
	if(!paused)solver->Execute(physics);

	axis->Draw(axisShader, camera->GetViewProjectionMatrix());
	bed->Draw(modelShader, camera->GetViewProjectionMatrix());
	//glDisable(GL_DEPTH_TEST);
	if(drawMesh) voxels->DrawInstanced(voxelShader, camera->GetViewProjectionMatrix(), nodeCount);
	//glEnable(GL_DEPTH_TEST);


}

void ExampleLayer::simulate(){

	if (!paused) {
		physics->Use();
		physics->SetDouble("dt", dt);
		physics->SetUniform3f("hpos", u);
		t += float(dt);

		u += v;
		if (u.x + speed >= domainWidth || u.x <= 0) {
			v.x = 0;
			v.y = speed;
		}

		if (u.y + speed > ((line + 1) * 2) * (domainWidth / sqNodeCount)) {
			line++;
			v.x = ((line % 2 == 0) ? speed : -speed);
			v.y = 0;
		}

		if (u.y + speed >= domainWidth) {
			line = 0;

			v.y = 0;
			u.x = 0;
			u.y = 0;
			v = glm::vec3(speed, 0, 0);
			u += glm::vec3(0, 0, (domainWidth / sqNodeCount)*10.0);
		}


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
		if (ImGui::SmallButton("Pause simulation"))
			paused = !paused;
	}

	

	ImGui::Checkbox("Draw Mesh", &drawMesh);
	ImGui::Checkbox("Draw Wireframe", &drawWiredMesh);

	if (ImGui::SmallButton("Reset simulation")) {
		solver->Execute(init); //init position using init compute shader
		t = 0;
		u = glm::vec3(0, 0, 0);
		v = glm::vec3(speed, 0, 0);
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
