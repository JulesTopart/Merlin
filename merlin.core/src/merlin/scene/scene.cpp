#include "glpch.h"
#include "scene.h"
#include "merlin/utils/modelLoader.h"

namespace Merlin {


	Scene::Scene(std::string name) : RenderableObject(name){
		_rootNode = createShared<RenderableObject>(name);
		_currentNode = _rootNode;
	}

	const std::list<Shared<RenderableObject>>& Scene::nodes() const {
		return _rootNode->children();
	}

	void Scene::setCamera(Shared<Camera> camera) {
		_camera = camera;
	}

	Camera& Scene::camera() {
		return *_camera;
	}

	void Scene::add(Shared<ParticleSystem> ps) {
		_currentNode->addChild(ps);
	}

	void Scene::add(Shared<deprecated_GenericParticleSystem> ps) {
		_currentNode->addChild(ps);
	}

	void Scene::add(Shared<Model> mdl) {
		_currentNode->addChild(mdl);
	}

	void Scene::add(Shared<Light> light) {
		_currentNode->addChild(light);
	}

	void Scene::add(Shared<SkyBox> sky) {
		_currentNode->addChild(sky);
	}

	void Scene::add(Shared<TransformObject> t) {
		_currentNode->addChild(t);
	}

	
}
