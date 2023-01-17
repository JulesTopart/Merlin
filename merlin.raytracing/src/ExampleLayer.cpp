#include "ExampleLayer.h"
#include "glfw/include/GLFW/glfw3.h"
#include "Merlin/Core/Input.h"

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

std::vector<Ray> parseRays(std::vector<float> data) {
	std::vector<Ray> rays;
	rays.resize(data.size()/3);
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
			Facet fc;
			fc.id = f.size();
			fc.p1 = vertices[fIndices[0]].position;
			fc.p2 = vertices[fIndices[1]].position;
			fc.p3 = vertices[fIndices[2]].position;
			fc.normal = vertices[fIndices[0]].normal;
			if (data[i] != GLuint(-1)) { //Quad
				fIndices[index] = (data[i] - 1);
				fc.p4 = vertices[fIndices[3]].position;
			}
			else {
				fIndices[index] = GLuint(-1);//Triangle
				fc.p4 = glm::vec3(-1.0);
			}
			f.push_back(fc);
			index = 0;

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
	std::vector<GLuint> indices_data;//v0, v1, v2, v4 (v4 = -1) It's Triangle
	std::vector<float> vertices_data; //xyz + temperature
	std::vector<float> rays_data;//xyz
	Vertices vertices;
	Indices indices;
	std::vector<Facet> facets;
	//std::vector<Ray> rayArray;//Now in the header

	//LoadBinaryFile<float>("assets/geometry/rayons.float3", rays_data);
	LoadBinaryFile<float>("assets/geometry/rayons.12800.float3", rays_data);
	LoadBinaryFile<float>("assets/geometry/vertices.float3", vertices_data);
	LoadBinaryFile<GLuint>("assets/geometry/indices.uint", indices_data);

	vertices = parseVerticies(vertices_data);
	indices = parseIndices(vertices, indices_data);
	facets = parseFacets(vertices, indices_data);
	rayArray = parseRays(rays_data);

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

	mouseCastShader = std::make_shared<Shader>("raycastShader");
	mouseCastShader->Compile(
		"assets/shaders/raycast.vert.glsl",
		"assets/shaders/raycast.frag.glsl"
	);

	modelShader = std::make_shared<Shader>("model");
	modelShader->Compile(
		"assets/shaders/model.vert.glsl",
		"assets/shaders/model.frag.glsl"
	);


	// -- Load models --
	axis = Primitive::CreateCoordSystem();
	sphere = Primitive::CreateSphere(0.1, 40, 40);
	model = CreateShared<Primitive>(vertices, indices);
	mouseRay = Primitive::CreateLine(1, glm::vec3(1, 0, 0));
	model->SetDrawMode(GL_TRIANGLES);

	// ---- Init Computing ----
	rays = CreateShared<ParticleSystem>("rays", rayArray.size());
	rays->SetThread(128);
	Shared<Primitive> line = Primitive::CreateLine(1, glm::vec3(1, 0, 0));
	//line->SetDrawMode(GL_POINTS);
	rays->SetPrimitive(line);
	

	// ---- GPU Data ----	
	//Load to GPU
	rayBuffer = CreateShared<SSBO>("RayBuffer");
	rayBuffer->Allocate<Ray>(rayArray);
	rayBuffer->SetBindingPoint(1);

	facetBuffer = CreateShared<SSBO>("FacetBuffer");
	facetBuffer->Allocate<Facet>(facets);
	facetBuffer->SetBindingPoint(2);

	//Programs
	init = CreateShared<ComputeShader>("init");
	init->Compile("assets/shaders/init.cs.glsl");
	raytracing = CreateShared<ComputeShader>("raytracing");
	raytracing->Compile("assets/shaders/raytracing.cs.glsl");

	rays->AddStorageBuffer(rayBuffer);
	rays->AddStorageBuffer(facetBuffer);

	rays->AddComputeShader(init);
	rays->AddComputeShader(raytracing);

	glm::vec3 TMRT_origin = glm::vec3(30.5, 10.5, 1.5);

	init->Use();
	//rays->Translate(TMRT_origin);
	init->SetUniform3f("origin", TMRT_origin);

	raytracing->Use();
	raytracing->SetUniform3f("origin", TMRT_origin);
	raytracing->SetUInt("size", facets.size());
	rays->Execute(init);

	//saveRays();



	//Set uniforms of rendering shaders
	cameraController.GetCamera().SetPosition(TMRT_origin);

	modelShader->Use();
	modelShader->SetUniform3f("lightPos", glm::vec3(30,30,200));
	modelShader->SetUniform3f("lightColor", glm::vec3(1, 0.8, 0.8));
	modelShader->SetUniform3f("viewPos", cameraController.GetCamera().GetPosition());
	modelShader->SetFloat("shininess", 1.0f);

	mouseCastShader->Use();
	mouseCastShader->SetUniform3f("start", glm::vec3(0));
	mouseCastShader->SetUniform3f("end", glm::vec3(0));

}

void ExampleLayer::saveRays() {
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

void ExampleLayer::OnEvent(Event& e){
	cameraController.OnEvent(e);


	if (e.IsInCategory(EventCategory::EventCategoryMouse)) {
		EventDispatcher dispatcher(e);
		if (e.GetEventType() == EventType::MouseButtonPressed) {
			dispatcher.Dispatch<MouseButtonPressedEvent>(GLCORE_BIND_EVENT_FN(ExampleLayer::OnClick));
		}
		if (e.GetEventType() == EventType::MouseButtonReleased) {
			dispatcher.Dispatch<MouseButtonReleasedEvent>(GLCORE_BIND_EVENT_FN(ExampleLayer::OnRelease));
		}
	}
}


bool ExampleLayer::OnClick(MouseButtonPressedEvent& e) {
	if (e.GetMouseButton() == MRL_MOUSE_BUTTON_LEFT) {//Left click
		glm::vec2 mouse = glm::vec2(Input::GetMouseX(), Input::GetMouseX());
		float x = (2.0f * mouse.x) / _width - 1.0f;
		float y = 1.0f - (2.0f * mouse.y) / _height;
		float z = 1.0f;
		glm::vec3 ray_nds = glm::vec3(x, y, z);
		glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0, 1.0);
		glm::vec4 ray_eye = glm::inverse(cameraController.GetCamera().GetProjectionMatrix()) * ray_clip;
		ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);
		glm::vec3 ray_wor = (glm::inverse(cameraController.GetCamera().GetViewMatrix()) * ray_eye);
		// don't forget to normalise the vector at some point
		ray_wor = glm::normalize(ray_wor);
		ray_wor = glm::vec3(ray_wor.x, ray_wor.y, ray_wor.z); //Dir
		mouseCastShader->Use();
		mouseCastShader->SetUniform3f("start", cameraController.GetCamera().GetPosition());
		mouseCastShader->SetUniform3f("end", cameraController.GetCamera().GetPosition() + ray_wor * 100.0f);
		return false;
	}
}

bool ExampleLayer::OnRelease(MouseButtonReleasedEvent& e) {
	return false;
}

void ExampleLayer::OnUpdate(Timestep ts){
	updateFPS(ts);
	cameraController.OnUpdate(ts);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Specify the color of the background
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// Clean the back buffer and depth buffer

	axis->Draw(axisShader, cameraController.GetCamera().GetViewProjectionMatrix());
	if(_drawGeom) model->Draw(modelShader, cameraController.GetCamera().GetViewProjectionMatrix());
	if(_drawRaycast) mouseRay->Draw(mouseCastShader, cameraController.GetCamera().GetViewProjectionMatrix());
	//sphere->Draw(modelShader, cameraController.GetCamera().GetViewProjectionMatrix());
	if (_drawRays) rays->Draw(rayShader, cameraController.GetCamera().GetViewProjectionMatrix());
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

	if (ImGui::ArrowButton("Run simulation", 1)) {
		Console::info("Raytracing") << "Starting..." << Console::endl;
		double time = (double)glfwGetTime();
		rays->Execute(raytracing);
		double detla = (double)glfwGetTime() - time;
		Console::success("Raytracing") << "Computation finished in " << detla << "s (" << detla*1000.0 << " ms)" << Console::endl;
	}
	if (ImGui::Button("Reset simulation")) {
		rayBuffer->Allocate(rayArray);
		rays->Execute(init);
	}

	ImGui::Checkbox("Draw Geometry", &_drawGeom);
	ImGui::Checkbox("Draw Rays", &_drawRays);

	if (ImGui::DragFloat3("Camera position", &camera_translation.x, -100.0f, 100.0f)) {
		cameraController.GetCamera().SetPosition(camera_translation);
	}
	if (ImGui::SliderFloat("Camera speed", &camera_speed, 0.0, 100.0f)) {
		cameraController.SetCameraSpeed(camera_speed);
	}
	ImGui::End();
}
