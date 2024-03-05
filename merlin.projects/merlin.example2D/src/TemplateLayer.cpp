#include "TemplateLayer.h"

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
	renderer.enableSampleShading();
	renderer.setBackgroundColor(0.203, 0.203, 0.203, 1.0);

	//modelShader = Shader::create("default", "assets/common/shaders/default.model.vert", "assets/common/shaders/default.model.frag");
	modelShader = Shader::create("model", "assets/common/shaders/default.model.vert", "assets/common/shaders/default.model.frag");
	modelShader->noTexture();
	renderer.addShader(modelShader);

	Shared<Model> model = Model::create("sphere1", Primitives::createSphere(0.5, 40, 40));
	model->setMaterial("gold");
	model->setShader("model");
	scene.add(model);

	light = Model::create("light", Primitives::createSphere(0.05));
	Shared<Material> lightMat = createShared<Material>("lightMat");
	lightMat->setAmbient(glm::vec3(1));
	lightMat->setDiffuse(glm::vec3(1));
	lightMat->setSpecular(glm::vec3(1));
	lightMat->setShininess(0.1);
	light->setMaterial(lightMat);

	light->translate(glm::vec3(1, 1, 0));

	scene.add(light);

	modelShader->use();
	modelShader->setVec3("lightPos", light->position());
	modelShader->setVec4("lightColor", glm::vec4(1));

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
	modelShader->use();
	modelShader->setVec3("lightPos", light->position());

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
