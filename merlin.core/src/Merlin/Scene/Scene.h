#pragma once
#include <vector>

#include "Merlin/Scene/SceneNode.h"
#include "Merlin/Scene/Model.h"
#include "Merlin/Scene/Light.h"
#include "Camera.h"

namespace Merlin::Graphics {

	class Scene {
		Shared<SceneNode> _rootNode = nullptr;
		Shared<SceneNode> _currentNode = nullptr;
		Shared<Camera> _camera = nullptr;

	public:
		Scene(std::string name = "scene");
		~Scene() {};

		void SpawnModel(Shared<Model>, std::string name);
		void SpawnLight(Shared<Light>, std::string name);

		void SetCamera(Shared<Camera> camera);
		Camera& camera();

		const Shared<SceneNode>& nodes();
		// Enumerate all objects in the scene
		void Draw(const Camera& camera) const;



		inline static Shared<Scene> Create() { return CreateShared<Scene>(); };
	};
	
}