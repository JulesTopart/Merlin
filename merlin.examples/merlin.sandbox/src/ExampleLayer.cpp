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
	camera->setFarPlane(1000.0f);
	camera->setFOV(45.0f); //Use 90.0f as we are using cubemaps
	//camera->setPosition(glm::vec3(30.0f, -180.0f, 50.0f));
	camera->setPosition(glm::vec3(0, 9, 5.5));
	camera->setRotation(glm::vec3(0, 25, -90));
	//camera->setRotation(glm::vec3(0, 0, 90.0f));
	cameraController = createShared<CameraController3D>(camera);
}

ExampleLayer::~ExampleLayer(){}

void ExampleLayer::onAttach(){
	enableGLDebugging();
	Console::setLevel(ConsoleLevel::_TRACE);

	renderer.initialize();
	renderer.enableSampleShading();
	renderer.setBackgroundColor(0.703, 0.703, 0.703, 1.0);

	//Shared<Model> model = Model::create("plane", Primitives::createRectangle(50, 50));
	//Shared<Model> model = Model::create("sphere", Primitives::createSphere(5, 40, 40));
	Shared<Model> model = ModelLoader::loadModel("./assets/common/models/model.obj");
	//model->translate(glm::vec3(0, 0, 2.5));
	//model->rotate(glm::vec3(glm::pi<float>() / 2.0, 0, 0));
	//model->scale(glm::vec3(1));
	//model->meshes()[0]->calculateNormals();
	//model->setMaterial("pearl");

	//Shared<Model> floor = Model::create("floor", Primitives::createCube(5));
	//floor->setMaterial("gold");

	Shared<PhongMaterial> customMat = createShared<PhongMaterial>("custom");
	customMat->loadTexture("./assets/common/models/model.aldebo.jpg", TextureType::DIFFUSE);
	customMat->loadTexture("./assets/common/models/model.normal.jpg", TextureType::NORMAL);
	customMat->setAmbient(glm::vec3(0.25, 0.20725, 0.20725));
	customMat->setDiffuse(glm::vec3(1, 0.829, 0.829));
	customMat->setSpecular(glm::vec3(0.296648, 0.296648, 0.296648));
	customMat->setShininess(0.088);
	model->setMaterial(customMat);
	//model->setMaterial(customMat);
	/**/


	light = createShared<PointLight>("light");
	light->translate(glm::vec3(radius, radius, 20));
	light->setAttenuation(glm::vec3(1.0, 0.022, 0.0020));

	dirlight = createShared<DirectionalLight>("light", glm::vec3(-0.5f, 0.5f, -1.0f));
	dirlight->translate(glm::vec3(radius, radius,10));
	dirlight->setAttenuation(glm::vec3(1.0,0.022,0.0020));

	scene.add(light);
	scene.add(dirlight);
	scene.add(model);
	//scene.add(floor);
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

	/*
	x = light2->position().x;
	y = light2->position().y;
	light2->translate(glm::vec3(cos(t+ hpi) * radius - x, sin(t+ hpi) * radius - y, 0.0));
	*/
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
