#include "glpch.h"
#include "scene.h"
#include "merlin/utils/modelLoader.h"

namespace Merlin {


	Scene::Scene(std::string name) : RenderableObject(name){
		m_rootNode = createShared<RenderableObject>(name);
		m_currentNode = m_rootNode;
	}

	const std::list<Shared<RenderableObject>>& Scene::nodes() const {
		return m_rootNode->children();
	}

	void Scene::setCamera(Shared<Camera> camera) {
		m_camera = camera;
	}

	Camera& Scene::camera() {
		return *m_camera;
	}
	
	void Scene::add(Shared<ParticleSystem> ps) {
		m_currentNode->addChild(ps);
	}
	/*
	void Scene::add(Shared<deprecated_GenericParticleSystem> ps) {
		m_currentNode->addChild(ps);
	}*/

	void Scene::add(Shared<Model> mdl) {
		m_currentNode->addChild(mdl);
	}

	void Scene::add(Shared<Mesh> mesh) {
		m_currentNode->addChild(mesh);
	}

	void Scene::add(Shared<Light> light) {
		m_currentNode->addChild(light);
	}

	bool Scene::hasEnvironment() const{
		return m_environment != nullptr;
	}

	void Scene::setEnvironment(Shared<Environment> sky) {
		m_environment = sky;
	}

	Shared<Environment> Scene::getEnvironment() const {
		return m_environment;
	}

	void Scene::add(Shared<TransformObject> t) {
		m_currentNode->addChild(t);
	}

	
}
