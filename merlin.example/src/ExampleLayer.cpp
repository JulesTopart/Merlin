#include "ExampleLayer.h"

using namespace Merlin;
using namespace Merlin::Utils;
using namespace Merlin::Memory;
using namespace Merlin::Graphics;

#include <iostream>
#include <iomanip>

ExampleLayer::ExampleLayer(){
	Window* w = &Application::Get().GetWindow();
	int height = w->GetHeight();
	int width = w->GetWidth();
	camera = CreateShared<Camera>(width, height, Projection::Perspective);
	camera->setNearPlane(0.001f);
	camera->setFarPlane(3000.0f);
	camera->setFOV(45.0f); //Use 90.0f as we are using cubemaps
	camera->SetPosition(glm::vec3(-2.0f, 0.0f, 1.0f));
	cameraController = CreateShared<CameraController3D>(camera);
}

ExampleLayer::~ExampleLayer(){}

Shared<Mesh> GetModel() {
	//return ModelLoader::LoadModel("assets/models/cube.stl")->meshes()[0];
	Shared<Mesh> m = Primitives::CreateSphere(0.5, 40, 40);
	return m;
}

void ExampleLayer::OnAttach(){
	EnableGLDebugging();
	Console::SetLevel(ConsoleLevel::_INFO);

	renderer.Initialize();
	renderer.EnableSampleShading();
	renderer.SetBackgroundColor(0.203, 0.203, 0.203, 1.0);

	//modelShader = Shader::Create("default", "assets/common/shaders/default.model.vert", "assets/common/shaders/default.model.frag");
	modelShader = Shader::Create("pbr", "assets/common/shaders/pbr.model.vert", "assets/common/shaders/pbr.model.frag");
	modelShader->noTexture();
	renderer.AddShader(modelShader);

	std::vector<std::string> skyBoxPath = {
		"./assets/textures/skybox/right.jpg",
		"./assets/textures/skybox/left.jpg",
		"./assets/textures/skybox/bottom.jpg",
		"./assets/textures/skybox/top.jpg",
		"./assets/textures/skybox/front.jpg",
		"./assets/textures/skybox/back.jpg"
	};

	Shared<Shader> skyShader = Shader::Create("skybox", "assets/common/shaders/default.skybox.vert", "assets/common/shaders/default.skybox.frag");
	sky = CreateShared<SkyBox>("Sky", skyBoxPath);
	sky->SetShader(skyShader);
	scene.Add(sky);

	Shared<Model> model = Model::Create("sphere1", GetModel());
	model->SetMaterial("aluminum");
	model->SetShader("pbr");
	scene.Add(model);


	light = Model::Create("light", Primitives::CreateSphere(0.05));
	Shared<Material> lightMat = CreateShared<Material>("lightMat");
	lightMat->SetAmbient(glm::vec3(1));
	lightMat->SetDiffuse(glm::vec3(1));
	lightMat->SetSpecular(glm::vec3(1));
	lightMat->SetShininess(0.1);
	lightMat->disablePBR();
	light->SetMaterial(lightMat);



	light->Translate(glm::vec3(2.8, 2, 0.6));

	scene.Add(light);

	modelShader->Use();
	modelShader->SetVec3("lightPos", light->position());
	modelShader->SetVec4("lightColor", glm::vec4(1));

	scene.SetCamera(camera);
}

void ExampleLayer::OnDetach(){}

void ExampleLayer::OnEvent(Event& event){
	camera->OnEvent(event);
	cameraController->OnEvent(event);
}

float t = 0.0;

void ExampleLayer::OnUpdate(Timestep ts){
	cameraController->OnUpdate(ts);

	t += ts;
	float x = light->position().x;
	float y = light->position().y;
	light->Translate(glm::vec3(cos(t) - x, sin(t) - y, 0.0));
	modelShader->Use();
	modelShader->SetVec3("lightPos", light->position());

	renderer.Clear();
	renderer.RenderScene(scene, *camera);
}

void ExampleLayer::OnImGuiRender()
{
	ImGui::Begin("Camera");

	model_matrix_translation = camera->GetPosition();
	if (ImGui::DragFloat3("Camera position", &model_matrix_translation.x, -100.0f, 100.0f)) {
		camera->SetPosition(model_matrix_translation);

	}
	ImGui::End();

	// Define a recursive lambda function to traverse the scene graph
	std::function<void(const std::list<Shared<RenderableObject>>&)> traverseNodes = [&](const std::list<Shared<RenderableObject>>& nodes)
	{
		for (auto& node : nodes)
		{
			bool node_open = ImGui::TreeNode(node->name().c_str());
			if (node_open)
			{
				
				if (node != nullptr)
				{
					ImGui::Text(node->name().c_str());
				}
				
				// Draw the node's children
				traverseNodes(node->children());

				ImGui::TreePop();
			}
		}
	};

	// Draw the scene graph starting from the root node
	ImGui::Begin("Scene Graph");
	traverseNodes(scene.nodes());
	ImGui::End();
	


}
