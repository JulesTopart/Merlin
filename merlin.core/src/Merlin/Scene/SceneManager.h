#pragma once
#include <map>

#include "Merlin/Renderer/Mesh.h"
#include "Camera.h"

using namespace Merlin::Renderer;

namespace Merlin::Scene {

	/*
	class SceneManager {
		std::vector<std::shared_ptr<Mesh>> meshes;
		std::vector<std::shared_ptr<Shader>> shaders;

		std::shared_ptr<Camera> camera;

	public:
		SceneManager() {};
		~SceneManager() {};

		void Draw();
		void DrawObject(std::string name, Shader& shader);
		void SpawnObject(std::shared_ptr<Mesh>);
		void RemoveObject(std::string name);
		void ClearObjects();


		// Enumerate all objects in the scene
		void EnumerateObjects(std::function<void(std::shared_ptr<Mesh>)> callback);

		std::shared_ptr<Mesh> GetObjectByName(std::string name);

		void SpawnCube(std::string name);
		void SpawnAxis(std::string name);

		std::shared_ptr<Shader> CreateShader(std::string name);
		std::shared_ptr<Shader> GetShaderByName(std::string name);
		void RemoveShader(std::string name);

		Camera& GetCamera();

	};
	*/
}