#include "glpch.h"
#include "Scene.h"
#include "Merlin/Util/ModelLoader.h"

namespace Merlin::Graphics {


	Scene::Scene() {
		_rootNode = CreateShared<SceneNode>("scene");
		_currentNode = _rootNode;
	}


	const Shared<SceneNode>& Scene::nodes() {
		return _rootNode;
	}

	void Scene::SetCamera(Shared<Camera> camera) {
		_camera = camera;
	}

	Camera& Scene::camera() {
		return *_camera;
	}

	void Scene::SpawnObject(Shared<SceneObject> obj) {
		Shared<SceneNode> node = CreateShared<SceneNode>(obj->name());
		node->SetObject(obj);
		_currentNode->AddChild(node);
	}

	void RemoveObject(Shared<SceneNode> toremove) {
		for (int i = 0; toremove->parent()->children().size(); i++) {
			if (toremove->parent()->children()[i] == toremove){
				toremove->parent()->RemoveChild(toremove->parent()->children()[i]);
			}
		}
	}

	void Scene::SpawnCube(std::string name) {
		Shared<Model> axis = Utils::ModelLoader::LoadCube();
		SpawnModel(axis, "Cube");
	}

	void Scene::SpawnAxis(std::string name) {
		Shared<Model> axis = Utils::ModelLoader::LoadAxis();
		SpawnModel(axis, "CoordinateSystem");

	}

	void Scene::SpawnModel(Shared<Model> mdl, std::string name) {
		Shared<SceneNode> sn = CreateShared<SceneNode>(name);
		Shared<ModelObject> so = CreateShared<ModelObject>(mdl);
		sn->SetObject(so);
		_currentNode->AddChild(sn);
		_models.push_back(mdl);
	}

	void Scene::SpawnLight(Shared<Light> light, std::string name) {
		Shared<SceneNode> sn = CreateShared<SceneNode>(name);
		Shared<LightObject> so = CreateShared<LightObject>(light);
		sn->SetObject(so);
		_currentNode->AddChild(sn);
	}

	const std::vector<Shared<Model>>& Scene::GetModels() {
		return _models;
	}

	void Scene::ClearObjects() {
		_rootNode->Clear();
		_models.clear();
		_lights.clear();
	}

	void Scene::EnumerateObjects(std::function<void(Shared<SceneObject>)> callback)
	{
		// Define a recursive lambda function to traverse the scene graph
		std::function<void(const std::vector<Shared<SceneNode>>&)> traverseNodes = [&](const std::vector<Shared<SceneNode>>& nodes)
		{
			for (auto& node : nodes){
				if (node->hasObject())
				{
					auto model = std::dynamic_pointer_cast<SceneObject>(node->object());
					if (model != nullptr)
					{
						callback(model);
					}
				}

				traverseNodes(node->children());
			}
		};

		// Traverse the scene graph starting from the root node
		traverseNodes(_rootNode->children());
	}

	Shared<SceneObject> Scene::GetObjectByName(std::string name) {
		// Define a recursive lambda function to traverse the scene graph
		std::function<Shared<SceneNode>(const std::vector<Shared<SceneNode>>&, const std::string&)> traverseNodes =
			[&](const std::vector<Shared<SceneNode>>& nodes, const std::string& name) -> Shared<SceneNode>
		{
			for (auto& node : nodes)
			{
				if (node->name() == name)
				{
					return node;
				}

				auto result = traverseNodes(node->children(), name);
				if (result)
				{
					return result;
				}
			}

			return nullptr;
		};

		// Traverse the scene graph starting from the root node
		auto result =  traverseNodes(_rootNode->children(), name);
		if(result == nullptr){
			Console::info("SceneManager") << "Node " << name << " not found" << Console::endl;
		}
		else {
			if (result->hasObject()) return result->object();
			else {
				Console::info("SceneManager") << "Node " << name << " not found" << Console::endl;
				return nullptr;
			}
		}
	}

	
}
