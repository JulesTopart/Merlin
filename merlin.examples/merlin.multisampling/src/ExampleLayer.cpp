#include "ExampleLayer.h"

using namespace Merlin;
using namespace Merlin::Utils;
using namespace Merlin::Graphics;

#include <iostream>
#include <iomanip>

ExampleLayer::ExampleLayer() {
	Window* w = &Application::Get().GetWindow();
	_height = w->GetHeight();
	_width = w->GetWidth();
	camera = CreateShared<Camera>(_width, _height, Projection::Perspective);
	camera->setNearPlane(0.001f);
	camera->setFarPlane(3000.0f);
	camera->setFOV(45.0f); //Use 90.0f as we are using cubemaps
	camera->SetPosition(glm::vec3(-2.0f, 0.0f, 1.0f));
	cameraController = CreateShared<CameraController3D>(camera);
}

ExampleLayer::~ExampleLayer() {}

Shared<Mesh> GetModel() {
	//return ModelLoader::LoadModel("assets/models/cube.stl")->meshes()[0];
	return Primitives::CreateSphere(0.5, 30, 30);
}

void ExampleLayer::LoadScene() {
	Shared<Model> model1 = Model::Create("sphere1", GetModel());
	Shared<Model> model2 = Model::Create("sphere2", GetModel());
	Shared<Model> model3 = Model::Create("sphere3", GetModel());
	Shared<Model> model4 = Model::Create("sphere4", GetModel());
	Shared<Model> model5 = Model::Create("sphere5", GetModel());
	Shared<Model> model6 = Model::Create("sphere6", GetModel());
	Shared<Model> model7 = Model::Create("sphere7", GetModel());
	Shared<Model> model8 = Model::Create("sphere8", GetModel());
	Shared<Model> model9 = Model::Create("sphere9", GetModel());
	Shared<Model> model10 = Model::Create("sphere10", GetModel());
	Shared<Model> model11 = Model::Create("sphere11", GetModel());
	Shared<Model> model12 = Model::Create("sphere12", GetModel());
	Shared<Model> model13 = Model::Create("sphere13", GetModel());
	Shared<Model> model14 = Model::Create("sphere14", GetModel());
	Shared<Model> model15 = Model::Create("sphere15", GetModel());
	Shared<Model> model16 = Model::Create("sphere16", GetModel());
	Shared<Model> model17 = Model::Create("sphere17", GetModel());
	Shared<Model> model18 = Model::Create("sphere18", GetModel());
	Shared<Model> model19 = Model::Create("sphere19", GetModel());
	Shared<Model> model20 = Model::Create("sphere20", GetModel());
	Shared<Model> model21 = Model::Create("sphere21", GetModel());
	Shared<Model> model22 = Model::Create("sphere22", GetModel());
	Shared<Model> model23 = Model::Create("sphere23", GetModel());
	Shared<Model> model24 = Model::Create("sphere24", GetModel());
	Shared<Model> floor = Model::Create("floor", Primitives::CreateCube(10, 6, 0.1));
	light = Model::Create("light", Primitives::CreateCube(0.2));

	model1->SetMaterial("emerald");
	model2->SetMaterial("jade");
	model3->SetMaterial("obsidian");
	model4->SetMaterial("pearl");
	model5->SetMaterial("ruby");
	model6->SetMaterial("turquoise");
	model7->SetMaterial("brass");
	model8->SetMaterial("bronze");
	model9->SetMaterial("chrome");
	model10->SetMaterial("copper");
	model11->SetMaterial("gold");
	model12->SetMaterial("silver");
	model13->SetMaterial("black plastic");
	model14->SetMaterial("cyan plastic");
	model15->SetMaterial("green plastic");
	model16->SetMaterial("red plastic");
	model17->SetMaterial("white plastic");
	model18->SetMaterial("yellow plastic");
	model19->SetMaterial("black rubber");
	model20->SetMaterial("cyan rubber");
	model21->SetMaterial("green rubber");
	model22->SetMaterial("red rubber");
	model23->SetMaterial("white rubber");
	model24->SetMaterial("yellow rubber");

	Shared<Material> lightMat = CreateShared<Material>("lightMat");
	lightMat->SetAmbient(glm::vec3(1));
	lightMat->SetDiffuse(glm::vec3(1));
	lightMat->SetSpecular(glm::vec3(1));
	lightMat->SetShininess(1);

	Shared<Material> floorMat = CreateShared<Material>("floorMat");
	floorMat->SetAmbient(glm::vec3(0.2));
	floorMat->SetDiffuse(glm::vec3(0.4));
	floorMat->SetSpecular(glm::vec3(0.2));
	floorMat->SetShininess(0.2);
	//floorMat->LoadTexture("./assets/textures/wall.jpg");

	light->SetMaterial(lightMat);
	floor->SetMaterial(floorMat);

	scene.Add(model1);
	scene.Add(model2);
	scene.Add(model3);
	scene.Add(model4);
	scene.Add(model5);
	scene.Add(model6);
	scene.Add(model7);
	scene.Add(model8);
	scene.Add(model9);
	scene.Add(model10);
	scene.Add(model11);
	scene.Add(model12);
	scene.Add(model13);
	scene.Add(model14);
	scene.Add(model15);
	scene.Add(model16);
	scene.Add(model17);
	scene.Add(model18);
	scene.Add(model19);
	scene.Add(model20);
	scene.Add(model21);
	scene.Add(model22);
	scene.Add(model23);
	scene.Add(model24);


	int offset = 0;
	float spacing = 1.2;
	for (auto& node : scene.nodes()) {
		if (node != nullptr)
		{
			node->Translate(glm::vec3(offset / 4 * spacing, (offset % 4) * spacing, 0));
			offset++;
		}

	}

	floor->Translate(glm::vec3(2.8, 2, -1));
	light->Translate(glm::vec3(2.8, 2, 2));

	scene.Add(floor);
	scene.Add(light);


	std::vector<std::string> skyBoxPath = {
		"./assets/textures/skybox/right.jpg",
		"./assets/textures/skybox/left.jpg",
		"./assets/textures/skybox/bottom.jpg",
		"./assets/textures/skybox/top.jpg",
		"./assets/textures/skybox/front.jpg",
		"./assets/textures/skybox/back.jpg"
	};

	std::vector<std::string> studioPath = {
		"./assets/textures/studio/pz.png",
		"./assets/textures/studio/nz.png",
		"./assets/textures/studio/ny.png",
		"./assets/textures/studio/py.png",
		"./assets/textures/studio/nx.png",
		"./assets/textures/studio/px.png"
	};

	Shared<Shader> skyShader = Shader::Create("skybox", "assets/shaders/skybox.vert.glsl", "assets/shaders/skybox.frag.glsl");
	Shared<SkyBox> sky = CreateShared<SkyBox>("Sky", skyBoxPath);
	sky->SetShader(skyShader);
	scene.Add(sky);

	modelShader->Use();
	modelShader->SetVec3("lightPos", light->position());

	scene.SetCamera(camera);
}


void ExampleLayer::OnAttach(){
	Window* w = &Application::Get().GetWindow();
	_height = w->GetHeight();
	_width = w->GetWidth();

	EnableGLDebugging();
	Console::SetLevel(ConsoleLevel::_INFO);

	renderer.Initialize();
	renderer.SetBackgroundColor(0.203, 0.203, 0.203, 1.0);

	renderer.EnableSampleShading();
	screen = std::make_shared<ScreenQuadRenderer>();
	
	// Create MSAA Frame Buffer Object
	msaa_fbo = std::make_shared<FBO>(_width, _height );
	msaa_fbo->Bind();

	//Create Texture  attachement
	msaa_fbo->AddColorAttachment(msaa_fbo->CreateTextureAttachment(GL_RGB, 16));
	// Create Render Buffer Object
	msaa_fbo->AddDepthStencilAttachment(msaa_fbo->CreateRenderBufferAttachment(GL_DEPTH24_STENCIL8, 16));

	// Create non MSAA Frame Buffer Object
	fbo = std::make_shared<FBO>(_width, _height);
	fbo->Bind();

	//Create Texture  attachement
	fbo->AddColorAttachment(fbo->CreateTextureAttachment(GL_RGB, 0));
	fbo->Unbind();
	
	modelShader = Shader::Create("default", "assets/shaders/model.vert", "assets/shaders/model.frag");
	renderer.AddShader(modelShader);
	
	LoadScene();
}

void ExampleLayer::OnDetach(){}

void ExampleLayer::OnEvent(Event& event) {
	camera->OnEvent(event);
	cameraController->OnEvent(event);

	if (event.GetEventType() == EventType::WindowResize) {
		Window* w = &Application::Get().GetWindow();
		_height = w->GetHeight();
		_width = w->GetWidth();

		msaa_fbo->Resize(_width, _height);
		fbo->Resize(_width, _height);
	}
}

float t = 0.0;

void ExampleLayer::OnUpdate(Timestep ts) {
	
	cameraController->OnUpdate(ts);
	updateFPS(ts);

	if (msaa) {
		// Bind the custom framebuffer
		msaa_fbo->Bind();
		glEnable(GL_DEPTH_TEST);
	}

	renderer.Clear();
	t += ts;
	float x = light->position().x;
	float y = light->position().y;
	light->Translate(glm::vec3(cos(t)*10 - x, sin(t)*10 - y, 0.0));
	modelShader->Use();
	modelShader->SetVec3("lightPos", light->position());

	renderer.RenderScene(scene, *camera);


	if (msaa) {
		// Bind the custom framebuffer
		msaa_fbo->Bind();
		//glEnable(GL_DEPTH_TEST);
		msaa_fbo->Bind(GL_READ_FRAMEBUFFER);
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		fbo->Bind(GL_DRAW_FRAMEBUFFER);
		//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		// Copy the multisampled framebuffer to the non-multisampled framebuffer, applying multisample resolve filters as needed
		glBlitFramebuffer(0, 0, _width, _height, 0, 0, _width, _height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
		// Bind the default framebuffer
	    msaa_fbo->Unbind();
		glDisable(GL_DEPTH_TEST); // prevents framebuffer rectangle from being discarded

		screen->Render(fbo->GetColorAttachment(0));

		glEnable(GL_DEPTH_TEST);
	}


}

void ExampleLayer::updateFPS(Timestep ts) {
	if (FPS_sample == 0) {
		FPS = ts;
	}
	else {
		FPS += ts;
	}
	FPS_sample++;
}


void ExampleLayer::OnImGuiRender()
{
	ImGui::Begin("Camera");

	if (FPS_sample > 0) {
		ImGui::LabelText("FPS", std::to_string(1.0f / (FPS / FPS_sample)).c_str());
		if (FPS_sample > 50) FPS_sample = 0;
	}

	model_matrix_translation = camera->GetPosition();
	if (ImGui::DragFloat3("Camera position", &model_matrix_translation.x, -100.0f, 100.0f)) {
		camera->SetPosition(model_matrix_translation);
	}
	ImGui::Checkbox("MSAA", &msaa);

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
}
