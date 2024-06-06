#include "Templatelayer.h"

using namespace Merlin;

#include <iostream>
#include <iomanip>

TemplateLayer::TemplateLayer(){}

TemplateLayer::~TemplateLayer(){}

void TemplateLayer::onAttach(){
	Layer2D::onAttach();

	renderer.initialize();
	renderer.disableDepthTest();
	renderer.disableEnvironment();

	renderer.setBackgroundColor(0.203, 0.203, 0.203, 1.0);
	renderer.showLights();


	Shared<Model> model = Model::create("sphere1", Primitives::createSphere(0.5, 40, 40));
	model->setMaterial("gold");
	scene.add(model);

	light = createShared<PointLight>("light", glm::vec3(0.5, 0, 0));
	light->setAmbient(glm::vec3(0.8));
	light->setDiffuse(glm::vec3(0.8));
	light->setSpecular(glm::vec3(0.4));
	light->setAttenuation(glm::vec3(0.05,0.001,0.3));
	scene.add(light);

}

void TemplateLayer::onDetach(){}

void TemplateLayer::onEvent(Event& event){
	Layer2D::onEvent(event);
}

float t = 0.0;

void TemplateLayer::onUpdate(Timestep ts){
	Layer2D::onUpdate(ts);

	t += ts;
	float x = light->position().x;
	float y = light->position().y;
	light->translate(glm::vec3(cos(t) - x, sin(t) - y, 0.0));

	renderer.clear();
	renderer.renderScene(scene, camera());
}

void TemplateLayer::onImGuiRender(){	
	
	ImGui::Begin("Camera");

	model_matrix_translation = camera().getPosition();
	if (ImGui::DragFloat3("Camera position", &model_matrix_translation.x, -100.0f, 100.0f)) {
		camera().setPosition(model_matrix_translation);
	}
	ImGui::End();


}
