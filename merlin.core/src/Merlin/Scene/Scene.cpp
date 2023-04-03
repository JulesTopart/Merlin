#include "glpch.h"
#include "Scene.h"
#include "Merlin/Util/ModelLoader.h"

namespace Merlin::Graphics {


	Scene::Scene(std::string name) {
		_rootNode = CreateShared<SceneNode>(name);
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

	void Scene::SpawnModel(Shared<Model> mdl, std::string name) {
		Shared<ModelNode> sn = CreateShared<ModelNode>(name, mdl);
		_currentNode->AddChild(sn);
	}

	void Scene::SpawnLight(Shared<Light> light, std::string name) {
		Shared<LightNode> sn = CreateShared<LightNode>(name, light);
		_currentNode->AddChild(sn);
	}

	void Scene::Draw(const Camera& camera) const{

		for (auto& node : _rootNode->children()) {
			node->Draw(camera);
		}

	}

	
}
