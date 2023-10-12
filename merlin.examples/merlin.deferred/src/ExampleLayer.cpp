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
	camera->setNearPlane(0.001f);
	camera->setFarPlane(3000.0f);
	camera->setFOV(45.0f); //Use 90.0f as we are using cubemaps
	camera->SetPosition(glm::vec3(-2.0f, 0.0f, 1.0f));
	cameraController = CreateShared<CameraController3D>(camera);
}

ExampleLayer::~ExampleLayer(){}


Shared<Mesh> GetModel() {
	//return ModelLoader::LoadModel("assets/models/cube.stl")->meshes()[0];
	return Primitives::CreateSphere(0.5, 30, 30);
}


void ExampleLayer::LoadScene() {
	Shared<Model> model1 = Model::Create("sphere1", GetModel());
	Shared<Model> model2 = Model::Create("sphere2", GetModel());
	Shared<Model> model3 = Model::Create("sphere3", GetModel());
	Shared<Model> model4 = Model::Create("sphere4", GetModel());

	light = Model::Create("light", Primitives::CreateCube(0.2));

	model1->SetMaterial("emerald");
	model2->SetMaterial("jade");
	model3->SetMaterial("obsidian");
	model4->SetMaterial("pearl");

	Shared<Material> lightMat = CreateShared<Material>("lightMat");
	lightMat->SetAmbient(glm::vec3(1));
	lightMat->SetDiffuse(glm::vec3(1));
	lightMat->SetSpecular(glm::vec3(1));
	lightMat->SetShininess(1);

	light->SetMaterial(lightMat);

	scene.Add(model1);
	scene.Add(model2);
	scene.Add(model3);
	scene.Add(model4);


	int offset = 0;
	float spacing = 1.2;
	for (auto& node : scene.nodes()) {
		if (node != nullptr)
		{
			node->Translate(glm::vec3(offset / 4 * spacing, (offset % 4) * spacing, 0));
			offset++;
		}

	}

	light->Translate(glm::vec3(2.8, 2, 2));

	scene.Add(light);


	std::vector<std::string> skyBoxPath = {
		"./assets/textures/skybox/right.jpg",
		"./assets/textures/skybox/left.jpg",
		"./assets/textures/skybox/bottom.jpg",
		"./assets/textures/skybox/top.jpg",
		"./assets/textures/skybox/front.jpg",
		"./assets/textures/skybox/back.jpg"
	};

	Shared<Shader> skyShader = Shader::Create("skybox", "assets/shaders/skybox.vert.glsl", "assets/shaders/skybox.frag.glsl");
	Shared<SkyBox> sky = CreateShared<SkyBox>("Sky", skyBoxPath);
	sky->SetShader(skyShader);
	scene.Add(sky);
}


void ExampleLayer::OnAttach(){
	Window* w = &Application::Get().GetWindow();
	_height = w->GetHeight();
	_width = w->GetWidth();

	EnableGLDebugging();
	Console::SetLevel(ConsoleLevel::_INFO);

	renderer.Initialize();
	renderer.SetBackgroundColor(0.203, 0.203, 0.203, 1.0);

	renderer.EnableDepthTest();
	renderer.EnableSampleShading();
	screen = std::make_shared<ScreenQuadRenderer>();

	// Create non MSAA Frame Buffer Object
	fbo = std::make_shared<FBO>(_width, _height);
	fbo->Bind();

	//Create Texture  attachement
	fbo->AddColorAttachment(fbo->CreateTextureAttachment(GL_RGBA, 0));//Position
	fbo->AddColorAttachment(fbo->CreateTextureAttachment(GL_RGBA, 0));//Normal
	fbo->AddColorAttachment(fbo->CreateTextureAttachment(GL_RGBA, 0));	 //Color + Specular
	//fbo->AddDepthStencilAttachment(fbo->CreateRenderBufferAttachment(GL_DEPTH24_STENCIL8, 0));	 ////Depth
	fbo->AddDepthStencilAttachment(fbo->CreateTextureAttachment(GL_DEPTH_COMPONENT, 0));	 //Depth
	fbo->SetDrawBuffer();
	fbo->Unbind();


	modelShader = Shader::Create("default", "assets/shaders/model.vert", "assets/shaders/model.frag");
	renderer.AddShader(modelShader);

	LoadScene();
}

void ExampleLayer::OnDetach(){}

void ExampleLayer::OnEvent(Event& event){
	camera->OnEvent(event);
	cameraController->OnEvent(event);
}

float t = 0.0;

void ExampleLayer::OnUpdate(Timestep ts){
	cameraController->OnUpdate(ts);

	// Bind the custom framebuffer
	fbo->Bind();
	
	renderer.Clear();
	t += ts;
	float y = light->position().y;
	light->Translate(glm::vec3(0.0, cos(t) - y, 0.0));
	modelShader->Use();
	modelShader->SetVec3("lightPos", light->position());
	renderer.RenderScene(scene, *camera);

	fbo->Unbind();
	if (bufferID < 3) screen->Render(fbo->GetColorAttachment(bufferID));//fbo->RenderAttachement(bufferID);
	else if (bufferID == 3) {
		screen->Render(fbo->GetDepthAttachement());
	}

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
				
				// Draw the node's children
				traverseNodes(node->children());

				ImGui::TreePop();
			}
		}
	};

	// Draw the scene graph starting from the root node
	ImGui::Begin("Scene Graph");
	traverseNodes(scene.nodes());
	ImGui::End();

	// Draw the scene graph starting from the root node
	ImGui::Begin("Buffer selection");
	ImGui::SliderInt("Buffer", &bufferID, 0, 5);
	ImGui::End();
	


}
