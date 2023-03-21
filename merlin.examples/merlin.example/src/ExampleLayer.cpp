#include "ExampleLayer.h"

using namespace Merlin;
using namespace Merlin::Utils;
using namespace Merlin::Graphics;

#include <iostream>
#include <iomanip>

ExampleLayer::ExampleLayer(){
	Window* w = &Application::Get().GetWindow();
	int height = w->GetHeight();
	int width = w->GetWidth();
	camera = CreateShared<Camera>(width, height, Projection::Perspective);
	camera->SetPosition(glm::vec3(-2.0f, 0.0f, 1.0f));
	cameraController = CreateShared<CameraController3D>(camera);
}

ExampleLayer::~ExampleLayer(){}

void ExampleLayer::OnAttach(){
	EnableGLDebugging();
	Console::SetLevel(ConsoleLevel::_INFO);

	scene = CreateShared<Scene>();
	Shared<Material> material = CreateShared<Material>();
	material->SetShader(CreateShared<Shader>("model","assets/shaders/model.vert.glsl", "assets/shaders/model.frag.glsl"));
	//material.SetTexture(TextureLoader::loadTexture("textures/teapot.png"));
	//model = ModelLoader::LoadModel("./assets/models/owl.stl");
	model = Model::Create(Primitives::CreateCube(1), material);

	scene->SetCamera(camera);
	scene->SpawnModel(model, "Owl");
	

	// Set the camera and scene for the renderer
	renderer.Initialize();
}

void ExampleLayer::OnDetach(){}

void ExampleLayer::OnEvent(Event& event){
	camera->OnEvent(event);
	cameraController->OnEvent(event);
}

void ExampleLayer::OnUpdate(Timestep ts){
	
	cameraController->OnUpdate(ts);

	// Render the scene
	renderer.Render(scene, *camera);

}

void ExampleLayer::OnImGuiRender()
{
	ImGui::Begin("Camera");

	model_matrix_translation = camera->GetPosition();
	if (ImGui::DragFloat3("Camera position", &model_matrix_translation.x, -100.0f, 100.0f)) {
		camera->SetPosition(model_matrix_translation);
	}
	ImGui::End();


	/*
	// Define a recursive lambda function to traverse the scene graph
	std::function<void(const std::vector<Shared<SceneNode>>&)> traverseNodes = [&](const std::vector<Shared<SceneNode>>& nodes)
	{
		for (auto& node : nodes)
		{
			bool node_open = ImGui::TreeNode(node->name().c_str());
			if (node_open)
			{
				// Draw the node's object
				if (node->hasObject())
				{
					auto mesh = std::dynamic_pointer_cast<SceneObject>(node->object());
					if (mesh != nullptr)
					{
						ImGui::Text(mesh->name().c_str());
						ImGui::SameLine();
						if (ImGui::Button("Edit"))
						{
							// Open a window to edit the mesh
						}
					}
				}

				// Draw the node's children
				traverseNodes(node->children());

				ImGui::TreePop();
			}
		}
	};

	// Draw the scene graph starting from the root node
	ImGui::Begin("Scene Graph");
	traverseNodes(scene->nodes()->children());
	ImGui::End();
	*/


}
