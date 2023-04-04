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
	camera->SetPosition(glm::vec3(-2.0f, 0.0f, 1.0f));
	cameraController = CreateShared<CameraController3D>(camera);
}

ExampleLayer::~ExampleLayer(){}

void ExampleLayer::OnAttach(){
	EnableGLDebugging();
	Console::SetLevel(ConsoleLevel::_INFO);

	renderer.Initialize();

	// Load the model and create a SceneNode from a GLTF file using Assimp
	Shared<Shader> sh = Shader::Create("assets/shaders/default.vert.glsl", "assets/shaders/default.frag.glsl");
	//ShaderLibrary::Add("default", sh);
	//scene.nodes()->AddChild(ModelLoader::LoadModel("assets/models/mando/scene.gltf"));

	Shared<ModelNode> mdl = CreateShared<ModelNode>("cone", ModelLoader::LoadModel("assets/models/cube.stl"));
	scene.nodes()->AddChild(mdl);
	// Add the model to the scene
	scene.SetCamera(camera);
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


	
	// Define a recursive lambda function to traverse the scene graph
	std::function<void(const std::list<Shared<SceneNode>>&)> traverseNodes = [&](const std::list<Shared<SceneNode>>& nodes)
	{
		for (auto& node : nodes)
		{
			bool node_open = ImGui::TreeNode(node->name().c_str());
			if (node_open)
			{
				
				if (node != nullptr)
				{
					ImGui::Text(node->name().c_str());
					ImGui::SameLine();
					if (ImGui::Button("Edit"))
					{
						// Open a window to edit the mesh
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
	traverseNodes(scene.nodes()->children());
	ImGui::End();
	


}
