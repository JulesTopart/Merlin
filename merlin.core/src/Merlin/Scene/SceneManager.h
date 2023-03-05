#pragma once
#include <vector>

#include "Merlin/Scene/SceneNode.h"
#include "Merlin/Scene/Model.h"
#include "Merlin/Scene/Light.h"
#include "Camera.h"

using namespace Merlin::Renderer;

namespace Merlin::Scene {

	class SceneManager {
		SceneNode _root;
		Shared<Camera> _camera;

	public:
		SceneManager() : _root("Scene") {};
		~SceneManager() {};

		void Draw(Shader& shader);
		void DrawObject(std::string name, Shader& shader);
		void SpawnObject(Shared<SceneObject>);
		void RemoveObject(std::string name);
		void ClearObjects();

		void SetCamera(Shared<Camera> camera);
		Camera& camera();

		const SceneNode& nodes();

		// Enumerate all objects in the scene
		void EnumerateObjects(std::function<void(Shared<SceneObject>)> callback);

		Shared<SceneObject> GetObjectByName(std::string name);

		void SpawnCube(std::string name);
		void SpawnAxis(std::string name);

		

	};
	
}