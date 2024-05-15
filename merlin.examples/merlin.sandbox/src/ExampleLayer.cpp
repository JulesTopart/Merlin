#include "Examplelayer.h"

using namespace Merlin;

#include <iostream>
#include <iomanip>

const float radius = 3;

ExampleLayer::ExampleLayer(){
	Window* w = &Application::get().getWindow();
	int height = w->getHeight();
	int width = w->getWidth();
	camera = createShared<Camera>(width, height, Projection::Perspective);
	camera->setNearPlane(0.001f);
	camera->setFarPlane(3000.0f);
	camera->setFOV(45.0f); //Use 90.0f as we are using cubemaps
	//camera->setPosition(glm::vec3(30.0f, -180.0f, 50.0f));
	camera->setPosition(glm::vec3(0.1, -7, 1.5));
	camera->setRotation(glm::vec3(0, 0, 90.0f));
	//camera->setRotation(glm::vec3(0, 0, 90.0f));
	cameraController = createShared<CameraController3D>(camera);
}

ExampleLayer::~ExampleLayer(){}

void ExampleLayer::onAttach(){
	enableGLDebugging();
	Console::setLevel(ConsoleLevel::_TRACE);

	renderer.initialize();
	renderer.enableSampleShading();
	renderer.setBackgroundColor(0.503, 0.503, 0.503, 1.0);

	//Shared<Model> model = Model::create("sphere", Primitives::createSphere(0.5, 40, 40));
	Shared<Model> model = ModelLoader::loadModel("./assets/common/models/teapot.obj");
	model->setMaterial("jade");
	model->rotate(glm::vec3(90,0,0));
	scene.add(model);

	light = createShared<PointLight>("light");
	light2 = createShared<PointLight>("light2");
	light->translate(glm::vec3(radius, radius, radius/3.0));
	light->setAttenuation(glm::vec3(0.1, 0.01, 0.001));
	light2->translate(glm::vec3(-radius, -radius, radius/2.0));
	light2->setAmbient(glm::vec3(1.0, 0, 0));
	light2->setDiffuse(glm::vec3(1.0, 0, 0));
	light2->setAttenuation(glm::vec3(1.0, 0.1, 0.01));

	/*
	light = Model::create("light", Primitives::createSphere(0.05));
	
	Shared<PhongMaterial> lightMat = createShared<PhongMaterial>("lightMat");
	lightMat->setAmbient(glm::vec3(1));
	lightMat->setDiffuse(glm::vec3(1));
	lightMat->setSpecular(glm::vec3(1));
	lightMat->setShininess(0.1);
	light->setMaterial(lightMat);

	light->translate(glm::vec3(2.8, 2, 0.6));*/
	scene.add(light);
	//scene.add(light2);
	
	
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
	const float hpi = 3.14159265358;
	t += ts;
	float x = light->position().x;
	float y = light->position().y;
	light->translate(glm::vec3(cos(t)* radius - x, sin(t)* radius - y, 0.0));

	x = light2->position().x;
	y = light2->position().y;
	light2->translate(glm::vec3(cos(t+ hpi) * radius - x, sin(t+ hpi) * radius - y, 0.0));

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
