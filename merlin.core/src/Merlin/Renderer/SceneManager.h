#pragma once
#include <map>

#include "Mesh.h"
#include "Shader.h"
#include "../Util/Camera.h"

using namespace Merlin::Utils;

namespace Merlin::Renderer {
	class SceneManager {
		std::map<std::string, std::shared_ptr<Mesh>> meshes;
		std::map<std::string, std::shared_ptr<Shader>> shaders;
		std::map<std::string, std::shared_ptr<Camera>> cameras;

	public:
		SceneManager() {};
		~SceneManager();

		void Draw();
		void DrawObject(std::string name, Shader& shader);
		void SpawnObject(std::shared_ptr<Mesh>);
		void RemoveObject(std::string name);
		void ClearObjects();

		/*
		// Enumerate all objects in the scene
		void enumerateObjects(std::function<void(std::shared_ptr<Mesh>)> callback) {
			for (const auto& pair : m_meshes) {
				callback(pair.second);
			}
		}
		*/

		std::shared_ptr<Mesh> GetObjectByName(std::string name);

		void SpawnCube(std::string name);
		void SpawnAxis(std::string name);

		Shader& CreateShader(std::string name);
		Shader& GetShaderByName(std::string name);
		Shader& RemoveShader(std::string name);

		Camera& GetCamera(std::string name);
		Camera& AddCamera(std::string name);
		void SetDefaultCamera(std::string name);
		void SelectCamera(std::string name);

	};
}