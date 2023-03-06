#pragma once
#include <vector>

#include "Merlin/Scene/SceneNode.h"
#include "Merlin/Scene/Model.h"
#include "Merlin/Scene/Light.h"
#include "Camera.h"

namespace Merlin::Renderer {

	class Scene {
		SceneNode _rootNode;

		Shared<Camera> _camera;
		std::vector<Shared<Model>> _models;
		std::vector<Shared<Material>> _material;

	public:
		Scene() : _rootNode("Scene") {};
		~Scene() {};

		void ClearObjects();
		void SpawnObject(Shared<SceneObject>);
		void RemoveObjectByID(int id);
		
		void SpawnCube(std::string name);
		void SpawnAxis(std::string name);
		void SpawnModel(Shared<Model>, std::string name);
		void SpawnLight(Shared<Light>, std::string name);

		void SetCamera(Shared<Camera> camera);
		Camera& camera();

		const SceneNode& nodes();

		// Enumerate all objects in the scene
		void EnumerateObjects(std::function<void(Shared<SceneObject>)> callback);

		Shared<SceneObject> GetObjectByName(std::string name);



		

	};
	
}