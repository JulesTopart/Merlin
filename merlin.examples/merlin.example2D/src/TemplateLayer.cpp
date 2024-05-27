#include "Templatelayer.h"

using namespace Merlin;

#include <iostream>
#include <iomanip>

TemplateLayer::TemplateLayer(){
	Window* w = &Application::get().getWindow();
	int height = w->getHeight();
	int width = w->getWidth();
	camera = createShared<Camera>(width, height, Projection::Orthographic);
	camera->setNearPlane(0.0f);
	camera->setFarPlane(1.0f);
	camera->setFOV(45.0f); //Use 90.0f as we are using cubemaps
	camera->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
}

TemplateLayer::~TemplateLayer(){}

void TemplateLayer::onAttach(){
	enableGLDebugging();
	Console::setLevel(ConsoleLevel::_INFO);

	renderer.initialize();
	renderer.disableDepthTest();
	renderer.disableEnvironment();
	//renderer.disableShadows();
	renderer.enableSampleShading();
	renderer.setBackgroundColor(0.203, 0.203, 0.203, 1.0);
	renderer.showLights();


	Shared<Model> model = Model::create("sphere1", Primitives::createSphere(0.5, 40, 40));
	model->setMaterial("gold");
	scene.add(model);

	light = createShared<PointLight>("light", glm::vec3(0.5, 0, 0));
	light->setAmbient(glm::vec3(0.2));
	light->setDiffuse(glm::vec3(0.8));
	light->setSpecular(glm::vec3(0.4));
	light->setAttenuation(glm::vec3(0.05,0.001,0.3));
	scene.add(light);


	Shared<AmbientLight> amlight = createShared<AmbientLight>("amLight");
	amlight->setAmbient(glm::vec3(0.5));
	scene.add(amlight);

	scene.setCamera(camera);
}

void TemplateLayer::onDetach(){}

void TemplateLayer::onEvent(Event& event){
	camera->onEvent(event);
}

float t = 0.0;

void TemplateLayer::onUpdate(Timestep ts){

	t += ts;
	float x = light->position().x;
	float y = light->position().y;
	light->translate(glm::vec3(cos(t) - x, sin(t) - y, 0.0));

	renderer.clear();
	renderer.renderScene(scene, *camera);
}

void TemplateLayer::onImGuiRender()
{
	ImGui::Begin("Camera");

	model_matrix_translation = camera->getPosition();
	if (ImGui::DragFloat3("Camera position", &model_matrix_translation.x, -100.0f, 100.0f)) {
		camera->setPosition(model_matrix_translation);
	}
	ImGui::End();


}
