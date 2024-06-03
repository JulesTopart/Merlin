#include "AppLayer.h"

using namespace Merlin;

#include <iostream>
#include <iomanip>

const float radius = 3;

AppLayer::AppLayer(){
	Window* w = &Application::get().getWindow();
	int height = w->getHeight();
	int width = w->getWidth();
	camera = createShared<Camera>(width, height, Projection::Perspective);
	camera->setNearPlane(0.1f);
	camera->setFarPlane(100.0f);
	camera->setFOV(60); //Use 90.0f as we are using cubemaps
	camera->setPosition(glm::vec3(0.7, -7, 2.4));
	camera->setRotation(glm::vec3(0, 0, +90));
	cameraController = createShared<CameraController3D>(camera);
}

AppLayer::~AppLayer(){}

void AppLayer::setupScene() {
	Shared<Model> bunny = ModelLoader::loadModel("./assets/common/models/bunny.stl");
	bunny->meshes()[0]->smoothNormals();
	bunny->setMaterial("pearl");
	bunny->scale(0.2);
	bunny->translate(glm::vec3(0, 0, -0.5));
	scene.add(bunny);

	Shared<DirectionalLight>  dirlight;
	/**/
	dirlight = createShared<DirectionalLight>("light1", glm::vec3(-0.5f, 0.5f, -0.8f));
	dirlight->translate(dirlight->direction() * glm::vec3(-10));
	dirlight->setDiffuse(glm::vec3(0.5));
	dirlight->castShadow(false);
	scene.add(dirlight);
	/**/

	/**/
	dirlight = createShared<DirectionalLight>("light2", glm::vec3(0.5f, 0.5f, -0.8f));
	dirlight->translate(dirlight->direction() * glm::vec3(-10));
	dirlight->setDiffuse(glm::vec3(0.5));
	dirlight->castShadow(false);
	scene.add(dirlight);
	/**/

	/**/
	dirlight = createShared<DirectionalLight>("light3", glm::vec3(0.0f, -0.5f, -0.8f));
	dirlight->translate(dirlight->direction() * glm::vec3(-10));
	dirlight->setDiffuse(glm::vec3(0.5));
	dirlight->castShadow(false);
	scene.add(dirlight);
	/**/

	/**/
	Shared<AmbientLight> amLight = createShared<AmbientLight>("light4");
	amLight->setAmbient(glm::vec3(0.3));
	scene.add(amLight);
	/**/

	scene.add(Primitives::createFloor(50, 0.5));
	scene.setCamera(camera);
}

void AppLayer::setupPhysics() {


	ps = ParticleSystem::create("Particles", 0);
	//bs = ParticleSystem::create("Bins", 0);

	ps->addField<glm::vec4>("position");
	ps->addField<glm::vec4>("position_cpy");
	ps->addField<glm::vec4>("predicted_position");
	ps->addField<glm::vec4>("predicted_position_cpy");
	ps->addField<glm::vec4>("velocity");
	ps->addField<glm::vec4>("velocity_cpy");

	StagedComputeShader_Ptr solver = StagedComputeShader::create("solver", "assets/shaders/solver.comp", 6);
	ps->addProgram(solver);

	ps->setDisplayMode(ParticleSystemDisplayMode::MESH);
	Mesh_Ptr sphere = Primitives::createSphere(0.5, 20, 20);
	
	ps->setMesh(sphere);

	scene.add(ps);
}

void AppLayer::onAttach(){
	renderer.initialize();
	renderer.enableSampleShading();
	renderer.disableFaceCulling();
	renderer.setEnvironmentGradientColor(0.903, 0.803, 0.703);
	renderer.showLights();

	setupScene();
	setupPhysics();
}

void AppLayer::onDetach(){}

void AppLayer::onEvent(Event& event){
	camera->onEvent(event);
	cameraController->onEvent(event);
}

float t = 0.0;

void AppLayer::onUpdate(Timestep ts){
	cameraController->onUpdate(ts);
	renderer.clear();
	renderer.renderScene(scene, *camera);
}

void AppLayer::onImGuiRender()
{
	ImGui::Begin("Camera");

	model_matrix_translation = camera->getPosition();
	if (ImGui::DragFloat3("Camera position", &model_matrix_translation.x, -100.0f, 100.0f)) {
		camera->setPosition(model_matrix_translation);

	}
	ImGui::End();

	// Define a recursive lambda function to traverse the scene graph
	std::function<void(const std::list<Shared<RenderableObject>>&)> traverseNodes = [&](const std::list<Shared<RenderableObject>>& nodes){
		for (auto& node : nodes){
			bool node_open = ImGui::TreeNode(node->name().c_str());
			if (node_open){
				
				if (node != nullptr){
					ImGui::Text(node->name().c_str());
				}

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
