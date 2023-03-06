#include "glpch.h"
#include "Scene.h"

namespace Merlin::Renderer {

	void Scene::Draw(Shader& shader) {
		std::function<void(const Shared<SceneObject>&)> drawNode = [&](const Shared<SceneObject>& object) {
			object->Draw(shader,_camera->GetViewProjectionMatrix());
		};

		EnumerateObjects(drawNode);
	}

	const SceneNode& Scene::nodes() {
		return _root;
	}

	void Scene::SetCamera(Shared<Camera> camera) {
		_camera = camera;
	}

	Camera& Scene::camera() {
		return *_camera;
	}

	void Scene::DrawObject(std::string name, Shader& shader) {
		auto obj = GetObjectByName(name);
		if (obj != nullptr) obj->Draw(shader, _camera->GetViewProjectionMatrix());
	}

	void Scene::SpawnObject(Shared<SceneObject> obj) {
		Shared<SceneNode> node = CreateShared<SceneNode>(obj->name());
		node->SetObject(obj);
		_root.AddChild(node);
	}

	void Scene::RemoveObject(std::string name) {
		
	}

	void Scene::ClearObjects() {
	
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
		traverseNodes(_root.children());
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
		auto result =  traverseNodes(_root.children(), name);
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
