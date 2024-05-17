#include "Examplelayer.h"

using namespace Merlin;

#include <iostream>
#include <iomanip>

ExampleLayer::ExampleLayer(){
	Window* w = &Application::get().getWindow();
	int height = w->getHeight();
	int width = w->getWidth();
	camera = createShared<Camera>(width, height, Projection::Perspective);
	camera->setNearPlane(0.001f);
	camera->setFarPlane(1000.0f);
	camera->setFOV(45.0f); //Use 90.0f as we are using cubemaps
	camera->setPosition(glm::vec3(-3.0f, 0.0f, 0.3f));
	cameraController = createShared<CameraController3D>(camera);
}

ExampleLayer::~ExampleLayer(){}

Shared<Mesh> GetModel() {
	//return ModelLoader::loadModel("assets/models/cube.stl")->meshes()[0];
	Shared<Mesh> m = Primitives::createSphere(0.5, 40, 40);
	return m;
}

void ExampleLayer::onAttach(){
	enableGLDebugging();
	Console::setLevel(ConsoleLevel::_INFO);

	renderer.initialize();
	renderer.enableSampleShading();
	renderer.setBackgroundColor(0.203, 0.203, 0.203, 1.0);

	Shared<Model> model = Model::create("sphere1", GetModel());
	model->setMaterial("jade");
	scene.add(model);

	light = createShared<PointLight>("light", glm::vec3(1, 0, 1));
	scene.add(light);
		
	scene.setCamera(camera);
}

void ExampleLayer::onDetach(){}

void ExampleLayer::onEvent(Event& event){
	camera->onEvent(event);
	cameraController->onEvent(event);
}

float t = 0.0;

void ExampleLayer::onUpdate(Timestep ts){
	cameraController->onUpdate(ts);

	t += ts;
	float x = light->position().x;
	float y = light->position().y;
	light->translate(glm::vec3(cos(t) - x, sin(t) - y, 0.0));

	renderer.clear();
	renderer.renderScene(scene, *camera);
}

void ExampleLayer::onImGuiRender()
{
	ImGui::Begin("Camera");

	model_matrix_translation = camera->getPosition();
	if (ImGui::DragFloat3("Camera position", &model_matrix_translation.x, -100.0f, 100.0f)) {
		camera->setPosition(model_matrix_translation);

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
				
				// draw the node's children
				traverseNodes(node->children());

				ImGui::TreePop();
			}
		}
	};

	// draw the scene graph starting from the root node
	ImGui::Begin("Scene Graph");
	traverseNodes(scene.nodes());
	ImGui::End();
	


}
