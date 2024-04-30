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
	camera->setFarPlane(3000.0f);
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

	modelShader = Shader::create("default", "assets/common/shaders/default.model.vert", "assets/common/shaders/default.model.frag");
	modelShader = Shader::create("model", "assets/common/shaders/default.model.vert", "assets/common/shaders/default.model.frag");
	modelShader->noTexture();
	renderer.addShader(modelShader);

	
	std::vector<std::string> skyBoxPath = {
		"./assets/textures/skybox/right.jpg",
		"./assets/textures/skybox/left.jpg",
		"./assets/textures/skybox/bottom.jpg",
		"./assets/textures/skybox/top.jpg",
		"./assets/textures/skybox/front.jpg",
		"./assets/textures/skybox/back.jpg"
	};

	Shared<Shader> skyShader = Shader::create("skybox", "assets/common/shaders/default.skybox.vert", "assets/common/shaders/default.skybox.frag");
	sky = createShared<SkyBox>("Sky", skyBoxPath);
	sky->setShader(skyShader);
	scene.add(sky);

	Shared<Model> model = Model::create("sphere1", GetModel());
	model->setMaterial("jade");
	model->setShader("model");
	scene.add(model);

	
	light = Model::create("light", Primitives::createSphere(0.05));
	
	Shared<Material> lightMat = createShared<Material>("lightMat");
	lightMat->setAmbient(glm::vec3(1));
	lightMat->setDiffuse(glm::vec3(1));
	lightMat->setSpecular(glm::vec3(1));
	lightMat->setShininess(0.1);
	light->setMaterial(lightMat);
	light->translate(glm::vec3(2.8, 2, 0.6));
	scene.add(light);
	
	
	modelShader->use();
	modelShader->setVec3("lightPos", light->position());
	modelShader->setVec4("lightColor", glm::vec4(1));
	
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
	modelShader->use();
	modelShader->setVec3("lightPos", light->position());

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
