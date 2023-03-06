#include "ExampleLayer.h"

using namespace Merlin;
using namespace Merlin::Utils;
using namespace Merlin::Renderer;

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

ExampleLayer::~ExampleLayer(){

}

void ExampleLayer::OnAttach(){
	EnableGLDebugging();

	Console::SetLevel(ConsoleLevel::_INFO);

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_MULTISAMPLE);

	axisShader = std::make_shared<Shader>("axis");

	axisShader->Compile(
		"assets/shaders/axis.vert.glsl",
		"assets/shaders/axis.frag.glsl"
	);

	modelShader = std::make_shared<Shader>("model");

	modelShader->Compile(
		"assets/shaders/model.vert.glsl",
		"assets/shaders/model.frag.glsl"
	);
	
	axis = ModelLoader::LoadAxis();
	model = ModelLoader::LoadCube();
	//model = ModelLoader::LoadPlane("plane");
	//model->LoadTexture("assets/textures/wall.jpg");
	//model->mesh().translate(glm::vec3(0, 0, -1));

	scene.SetCamera(camera);
	Shared<ModelObject> obj = CreateShared<ModelObject>(model);
	obj->SetName("model");
	scene.SpawnObject(obj);








	// Create a renderer object
	Renderer renderer;

	// Create a camera object and set its position and orientation
	Camera camera;
	camera.setPosition(Vector3(0, 0, -5));
	camera.setOrientation(Quaternion::fromAxisAngle(Vector3(0, 1, 0), 0.0f));

	// Create a mesh object from a file
	Mesh mesh = Mesh::loadFromFile("models/teapot.obj");

	// Create a material object
	Material material;
	material.setShader(Shader::loadFromFile("shaders/basic.vert", "shaders/basic.frag"));
	material.setTexture(Texture::loadFromFile("textures/teapot.png"));

	// Create a scene object and add the mesh and material to it
	Scene scene;
	scene.addMesh(mesh);
	scene.addMaterial(material);

	// Set the camera and scene for the renderer
	renderer.setCamera(camera);
	renderer.setScene(scene);

	// Render the scene
	renderer.render();



}

void ExampleLayer::OnDetach(){
	model.reset();
}

void ExampleLayer::OnEvent(Event& event){
	camera->OnEvent(event);
	cameraController->OnEvent(event);
}

void ExampleLayer::OnUpdate(Timestep ts){
	
	cameraController->OnUpdate(ts);

	glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
	modelShader->Use();
	modelShader->SetVec3("light.position", glm::vec3(0,3,3));
	modelShader->SetVec3("light.direction", glm::vec3(0,0.2,-1));
	modelShader->SetFloat("light.constant_attenuation", 0.1);
	modelShader->SetFloat("light.linear_attenuation", 0.1);
	modelShader->SetFloat("light.quadratic_attenuation", 0.1);

	modelShader->SetVec3("light.ambient", glm::vec3(0, 0, -1));
	modelShader->SetVec3("light.diffuse", glm::vec3(0, 0, -1));
	modelShader->SetVec3("light.specular", glm::vec3(0, 0, -1));

	modelShader->SetFloat("light.inner_cutoff", 0.1);
	modelShader->SetFloat("light.outer_cutoff", 0.1);
	modelShader->SetFloat("type", 1); // 0 = directional, 1 = point, 2 = spot

	axis->Draw(*axisShader, camera->GetViewProjectionMatrix());
	scene.Draw(*modelShader);
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
	traverseNodes(scene.nodes().children());
	ImGui::End();
	


}
