#include "ExampleLayer.h"

using namespace Merlin;
using namespace Merlin::Utils;
using namespace Merlin::Renderer;

#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <limits>

template<typename T>
void LoadBinaryFile(std::string path, std::vector<T>& data) {
	std::ifstream binary(path, std::ios::binary | std::ios::in | std::ios::ate);
	
	if (!binary) {
		Console::error("Application") << "Error opening file: " << path << Console::endl;
		return;
	}
	auto size = binary.tellg();
	binary.seekg(0, std::ios::beg);
	data.resize(size/sizeof(T));
	binary.read(reinterpret_cast<char*>(data.data()), size);
	binary.close();
}

std::vector<Ray> parseRays(std::vector<float> data, int size) {
	std::vector<Ray> rays;
	rays.resize(size);
	size_t index = 0;
	for (size_t i = 0; i < rays.size(); i++) {
		rays[i].origin = glm::vec3(data[index], data[index + 1], data[index + 2]);
		index += 3;
	}
	return rays;
}

glm::vec3 computeFacetNormal(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
	// Uses p2 as a new origin for p1,p3
	auto u = p2 - p1;
	auto v = p3 - p1;
	
	auto x = u.y * v.z - u.z * v.y;
	auto y = u.z * v.x - u.x * v.z;
	auto z = u.x * v.y - u.y * v.x;
	return glm::vec3(x, y, z);
}

Vertices parseVerticies(std::vector<float> data) {
	Vertices vertices;
	vertices.resize(data.size()/4);
	int index = 0;
	for (size_t i = 0; i < vertices.size(); i++) {
		vertices[i].position = glm::vec3(data[index], data[index + 1], data[index + 2]);
		vertices[i].color = data[index + 3] == 1.0f ? glm::vec3(0.2f, 0.2f, 1.0f) : glm::vec3(0.3f,0.3f,0.3f);
		index += 4;
	}
	return vertices;
}

Indices parseIndices(Vertices& vertices, std::vector<GLuint> data) {
	Indices indices;
	size_t index = 0;
	for (size_t i = 0; i < data.size(); i++) {
		if (index < 3) {
			indices.push_back(data[i] - 1);
			if (index == 2) vertices[indices[indices.size() - 1]].normal = computeFacetNormal(vertices[indices[indices.size() -3]].position, vertices[indices[indices.size() -2]].position, vertices[indices[indices.size()-1]].position);
			if (index == 2) vertices[indices[indices.size() - 2]].normal = computeFacetNormal(vertices[indices[indices.size() -3]].position, vertices[indices[indices.size() -2]].position, vertices[indices[indices.size()-1]].position);
			if (index == 2) vertices[indices[indices.size() - 3]].normal = computeFacetNormal(vertices[indices[indices.size() -3]].position, vertices[indices[indices.size() -2]].position, vertices[indices[indices.size()-1]].position);
			index++;
		} else{
			if (data[i] != GLuint(-1)) { //Quad
				indices.push_back(data[i - 3] - 1);
				indices.push_back(data[i - 1] - 1);
				indices.push_back(data[i] - 1);
				vertices[indices[indices.size()-1]].normal = computeFacetNormal(vertices[indices[indices.size()-3]].position, vertices[indices[indices.size() - 2]].position, vertices[indices[indices.size()-1]].position);
				vertices[indices[indices.size()-2]].normal = computeFacetNormal(vertices[indices[indices.size()-3]].position, vertices[indices[indices.size() - 2]].position, vertices[indices[indices.size()-1]].position);
				vertices[indices[indices.size()-3]].normal = computeFacetNormal(vertices[indices[indices.size()-3]].position, vertices[indices[indices.size() - 2]].position, vertices[indices[indices.size()-1]].position);
			}
			index = 0;
		}
	}
	return indices;
}

std::vector<Facet> parseFacets(Vertices& vertices, std::vector<GLuint> data) {
	std::vector<Facet> f;
	size_t index = 0;
	GLuint fIndices[4];
	for (size_t i = 0; i < data.size(); i++){
		if (index < 3) {
			fIndices[index] = (data[i] - 1);
			index++;
		}
		else {
			if (data[i] != GLuint(-1)) //Quad
				fIndices[index] = (data[i] - 1);
			else 
				fIndices[index] = GLuint(-1);//Triangle
			index = 0;
			Facet fc;
			fc.id = f.size();
			fc.indices[0] = fIndices[0];
			fc.indices[1] = fIndices[1];
			fc.indices[2] = fIndices[2];
			fc.indices[3] = fIndices[3];
			fc.normal = vertices[fc.indices[2]].normal;
			f.push_back(fc);
		}
	}
	return f;
}


ExampleLayer::ExampleLayer() : cameraController(45.0f, 16.0f / 9.0f){
	cameraController.GetCamera().SetPosition(glm::vec3(30.5f, 30.5f, 1.5f));
}

ExampleLayer::~ExampleLayer(){
}

void ExampleLayer::OnAttach(){
	EnableGLDebugging();
	Window* wd = &Application::Get().GetWindow();
	_height = wd->GetHeight();
	_width = wd->GetWidth();

	Console::SetLevel(ConsoleLevel::_INFO);


	// -- Load Geometry--
	GLsizeiptr rayCount = 128;

	std::vector<GLuint> indices_data;//v0, v1, v2, v4 (v4 = -1) It's Triangle
	std::vector<float> vertices_data; //xyz + temperature
	std::vector<float> rays_data(rayCount * 3);//xyz
	Vertices vertices;
	Indices indices;
	std::vector<Facet> facets;
	std::vector<Ray> rayArray;

	LoadBinaryFile<float>("assets/geometry/rayons.float3", rays_data);
	LoadBinaryFile<float>("assets/geometry/vertices.float3", vertices_data);
	LoadBinaryFile<GLuint>("assets/geometry/indices.uint", indices_data);

	vertices = parseVerticies(vertices_data);
	indices = parseIndices(vertices, indices_data);
	facets = parseFacets(vertices, indices_data);
	rayArray = parseRays(rays_data, 128);

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

	rayShader = std::make_shared<Shader>("rayShader");
	rayShader->Compile(
		"assets/shaders/ray.vert.glsl",
		"assets/shaders/ray.frag.glsl"
	);

	modelShader = std::make_shared<Shader>("model");
	modelShader->Compile(
		"assets/shaders/model.vert.glsl",
		"assets/shaders/model.frag.glsl"
	);
	//Set uniforms
	modelShader->Use();
	modelShader->SetUniform3f("lightPos", glm::vec3(-200, 0, 200));
	modelShader->SetUniform3f("lightColor", glm::vec3(1, 1, 1));
	modelShader->SetUniform3f("viewPos", cameraController.GetCamera().GetPosition());
	modelShader->SetFloat("shininess", 1.0f);

	// -- Load models --
	axis = Primitive::CreateCoordSystem();
	sphere = Primitive::CreateSphere(0.1, 40, 40);
	model = CreateShared<Primitive>(vertices, indices);
	//model->SetDrawMode(GL_LINES);

	// ---- Init Computing ----
	rays = CreateShared<ParticleSystem>("rays", 128);
	rays->SetPrimitive(Primitive::CreateLine(1,glm::vec3(1,0,0)));
	

	// ---- GPU Data ----	
	//Load to GPU
	rayBuffer = CreateShared<SSBO>("RayBuffer");
	rayBuffer->Allocate<Ray>(rayArray);
	rayBuffer->SetBindingPoint(1);

	facetBuffer = CreateShared<SSBO>("FacetBuffer");
	facetBuffer->Allocate<Facet>(facets);
	facetBuffer->SetBindingPoint(2);

	vertexBuffer = CreateShared<SSBO>("VertexBuffer");
	vertexBuffer->Allocate<Vertex>(vertices);
	vertexBuffer->SetBindingPoint(3);

	//Programs
	init = CreateShared<ComputeShader>("init");
	init->Compile("assets/shaders/init.cs.glsl");
	raytracing = CreateShared<ComputeShader>("raytracing");
	raytracing->Compile("assets/shaders/raytracing.cs.glsl");

	rays->AddStorageBuffer(rayBuffer);
	rays->AddStorageBuffer(facetBuffer);
	rays->AddStorageBuffer(vertexBuffer);

	rays->AddComputeShader(init);
	rays->AddComputeShader(raytracing);


	glm::vec3 TMRT_origin = glm::vec3(30.5, 30.5, 1.5);

	init->Use();
	//rays->Translate(TMRT_origin);
	init->SetUniform3f("origin", TMRT_origin);

	raytracing->Use();
	raytracing->SetUniform3f("origin", TMRT_origin);
	raytracing->SetUInt("size", facets.size());

	rays->Execute(init);
	rays->Execute(raytracing);
	std::vector<Ray> result;
	result.resize(128);

	memcpy(result.data(), rayBuffer->Map(), result.size() * sizeof(Ray));


	std::ofstream file("file.bin", std::ios::binary);
	for (int i = 0; i < result.size(); i++) {
		int num = result[i].hitID;
		file.write((char*)&num, sizeof(num));
	}
	file.close();

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

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Specify the color of the background
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

	if (ImGui::ArrowButton("Run simulation", 1)) rays->Execute(raytracing);

	if (ImGui::DragFloat3("Camera position", &camera_translation.x, -100.0f, 100.0f)) {
		cameraController.GetCamera().SetPosition(camera_translation);
	}
	if (ImGui::SliderFloat("Camera speed", &camera_speed, 0.0, 100.0f)) {
		cameraController.SetCameraSpeed(camera_speed);
	}
	ImGui::End();
}
