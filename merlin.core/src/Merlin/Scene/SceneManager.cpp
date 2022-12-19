#include "glpch.h"
#include "SceneManager.h"
using namespace Merlin::Renderer;

namespace Merlin::Scene {


	/*
	void SceneManager::Draw() {
		for (const auto& mesh : meshes){
			Shader& shader = GetShaderByName(mesh.second->GetLinkedShaderName());
			mesh.second->Draw(shader, camera->GetViewMatrix());
		}
	}

	void SceneManager::DrawObject(std::string name, Shader& shader) {
	
	}

	void SceneManager::SpawnObject(std::shared_ptr<Mesh>) {
	
	}

	void SceneManager::RemoveObject(std::string name) {
	
	}

	void SceneManager::ClearObjects() {
	
	}

	
	// Enumerate all objects in the scene
	void SceneManager::EnumerateObjects(std::function<void(std::shared_ptr<Mesh>)> callback) {
		for (const auto& pair : meshes) {
			callback(pair.second);
		}
	}

	std::shared_ptr<Mesh> SceneManager::GetObjectByName(std::string name) {
		return meshes.at(name);
	}

	

	std::shared_ptr<Shader> SceneManager::CreateShader(std::string name) {
		std::shared_ptr<Shader> shader = std::make_shared<Shader>(name);
		shaders[name] = shader;
	}

	std::shared_ptr<Shader> SceneManager::GetShaderByName(std::string name) {
		if (shaders.count(name) > 0) {
			return shaders[name];
		}
		else {
			Console::error("SceneManager") << name << " not found" << Console::endl;
		}
	}

	void SceneManager::RemoveShader(std::string name) {
		if (shaders.count(name) > 0) {
			shaders.erase(name);
		}
		else {
			Console::error("SceneManager") << name << " not found" << Console::endl;
		}
	}

	*/
}
