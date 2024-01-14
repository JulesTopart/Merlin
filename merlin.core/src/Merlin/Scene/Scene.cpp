#include "glpch.h"
#include "Scene.h"
#include "Merlin/Util/ModelLoader.h"

namespace Merlin::Graphics {


	Scene::Scene(std::string name) : RenderableObject(name){
		_rootNode = CreateShared<RenderableObject>(name);
		_currentNode = _rootNode;
	}

	const std::list<Shared<RenderableObject>>& Scene::nodes() const {
		return _rootNode->children();
	}

	void Scene::SetCamera(Shared<Camera> camera) {
		_camera = camera;
	}

	Camera& Scene::camera() {
		return *_camera;
	}


	void Scene::Add(Shared<deprecated_GenericParticleSystem> ps) {
		_currentNode->AddChild(ps);
	}

	void Scene::Add(Shared<Model> mdl) {
		_currentNode->AddChild(mdl);
	}

	void Scene::Add(Shared<Light> light) {
		_currentNode->AddChild(light);
	}

	void Scene::Add(Shared<SkyBox> sky) {
		_currentNode->AddChild(sky);
	}

	void Scene::Add(Shared<TransformObject> t) {
		_currentNode->AddChild(t);
	}

	
}
