#include "ExampleLayer.h"

using namespace Merlin;
using namespace Merlin::Utils;
using namespace Merlin::Renderer;

#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>

template<typename T>
void LoadBinaryFile(std::string path, std::vector<T>& data) {
	std::ifstream binary(path, std::ios::binary);

	if (!binary) {
		Console::error("Application") << "Error opening file: " << path << Console::endl;
		return;
	}

	binary.read((char*)data.data(), data.size() * sizeof(T));
	binary.close();
}


ExampleLayer::ExampleLayer() : cameraController(45.0f, 16.0f / 9.0f){
	cameraController.GetCamera().SetPosition(glm::vec3(-2.0f, 0.0f, 0.0f));
}

ExampleLayer::~ExampleLayer(){
}

void ExampleLayer::OnAttach(){
	EnableGLDebugging();
	Window* wd = &Application::Get().GetWindow();
	_height = wd->GetHeight();
	_width = wd->GetWidth();

	Console::SetLevel(ConsoleLevel::_INFO);


	// ---- Init Rendering ----
	// Init OpenGL stuff
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);

	//Shaders
	axisShader = std::make_shared<Shader>("axis");
	axisShader->Compile(
		"assets/shaders/axis.vert.glsl",
		"assets/shaders/axis.frag.glsl"
	);

	rayShader = std::make_shared<Shader>("rays");
	rayShader->Compile(
		"assets/shaders/ray.vert.glsl",
		"assets/shaders/ray.frag.glsl"
	);

	modelShader = std::make_shared<Shader>("model");
	modelShader->Compile(
		"assets/shaders/model.vert.glsl",
		"assets/shaders/model.frag.glsl"
	);
	modelShader->Use();
	modelShader->SetUniform3f("lightPos", glm::vec3(2, 0, 3));
	modelShader->SetUniform3f("lightColor", glm::vec3(1, 1, 1));
	modelShader->SetUniform3f("viewPos", cameraController.GetCamera().GetPosition());
	modelShader->SetFloat("shininess", 1.0f);

	//Load models
	axis = Primitive::CreateCoordSystem();
	sphere = Primitive::CreateSphere(1, 40, 40);

	model = Primitive::CreateRectangle(2, 1);
	model->Translate(glm::vec3(0, 1, 0));
	model->Rotate(glm::vec3(3.1415926 / 2.0, 0, 0));


	// ---- Init Computing ----
	rays = CreateShared<ParticleSystem>("rays", 128);
	rays->SetPrimitive(Primitive::CreateLine(0.5,glm::vec3(1,0,0)));

	//Data
	rayBuffer = CreateShared<SSBO>("RayBuffer");

	std::vector<float> data;
	data.resize(128 * 3); // allocate memory for an array 
	LoadBinaryFile<float>("assets/geometry/rayons.float3", data);

	std::vector<Ray> rayArray;
	rayArray.resize(128);

	int index = 0;
	for (int i = 0; i < rayArray.size(); i++) {
		rayArray[i].origin = glm::vec3(data[index], data[index + 1], data[index + 2]);
		index += 3;
	}

	rayBuffer->Allocate<Ray>(rayArray);
	rayBuffer->SetBindingPoint(1);

	facetBuffer = CreateShared<SSBO>("FacetBuffer");
	facetBuffer->Allocate<Facet>(1);
	facetBuffer->SetBindingPoint(2);


	//Programs
	init = CreateShared<ComputeShader>("init");
	init->Compile("assets/shaders/init.cs.glsl");
	raytracing = CreateShared<ComputeShader>("raytracing");
	raytracing->Compile("assets/shaders/raytracing.cs.glsl");


	rays->AddStorageBuffer(rayBuffer);
	rays->AddComputeShader(init);
	rays->AddComputeShader(raytracing);

	rays->Execute(init);

}

void ExampleLayer::OnDetach(){}

void ExampleLayer::updateFPS(Timestep ts) {
	if (FPS_sample == 0) {
		FPS = ts;
	}else {
		FPS += ts;
	}
	FPS_sample++;
}

void ExampleLayer::OnEvent(Event& event){
	cameraController.OnEvent(event);
}

void ExampleLayer::OnUpdate(Timestep ts){
	updateFPS(ts);
	cameraController.OnUpdate(ts);

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f); // Specify the color of the background
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// Clean the back buffer and depth buffer

	axis->Draw(axisShader, cameraController.GetCamera().GetViewProjectionMatrix());
	model->Draw(modelShader, cameraController.GetCamera().GetViewProjectionMatrix());
	//sphere->Draw(modelShader, cameraController.GetCamera().GetViewProjectionMatrix());
	rays->Draw(rayShader, cameraController.GetCamera().GetViewProjectionMatrix());
}

void ExampleLayer::OnImGuiRender()
{
	ImGui::Begin("Infos");

	camera_translation = cameraController.GetCamera().GetPosition();
	camera_speed = cameraController.GetCameraSpeed();

	if (FPS_sample > 0) {
		ImGui::LabelText("FPS", std::to_string(1.0f / (FPS / FPS_sample)).c_str());
		if (FPS_sample > 50) FPS_sample = 0;
	}

	if (ImGui::ArrowButton("Run simulation", 1))

	if (ImGui::DragFloat3("Camera position", &camera_translation.x, -100.0f, 100.0f)) {
		cameraController.GetCamera().SetPosition(camera_translation);
	}
	if (ImGui::SliderFloat("Camera speed", &camera_speed, 0.0, 100.0f)) {
		cameraController.SetCameraSpeed(camera_speed);
	}
	ImGui::End();
}
