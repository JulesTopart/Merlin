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
		
		std::vector<Shared<Model>> _models;
		std::vector<Shared<Light>> _lights;
		std::vector<Shared<Material>> _material;

	public:
		Scene();
		~Scene() {};

		void ClearObjects();
		void SpawnObject(Shared<SceneObject>);
		
		void SpawnCube(std::string name);
		void SpawnAxis(std::string name);
		void SpawnModel(Shared<Model>, std::string name);
		void SpawnLight(Shared<Light>, std::string name);

		void SetCamera(Shared<Camera> camera);
		Camera& camera();

		const Shared<SceneNode>& nodes();
		const std::vector<Shared<Model>>& GetModels();

		// Enumerate all objects in the scene
		void EnumerateObjects(std::function<void(Shared<SceneObject>)> callback);

		Shared<SceneObject> GetObjectByName(std::string name);



		

	};
	
}