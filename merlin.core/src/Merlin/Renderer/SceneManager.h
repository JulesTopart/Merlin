#pragma once
#include <map>
//#include "opengl/Mesh.h"
#include "Shader.h"

namespace Merlin::Renderer {
	class SceneManager {
		std::map<std::string, Shader*> shaders;

	public:
		inline SceneManager() {};
		~SceneManager();

		void Create(int width, int height);
		void Resize(int width, int height);

		void Draw();
		void DrawObject(std::string name, Shader& shader);
		void RemoveObject(std::string name);

		//Mesh& SpawnPlane(std::string name);
		//Mesh& SpawnCube(std::string name);
		//Mesh& SpawnMesh(std::string name, QString path);
		//Mesh& SpawnAxis(std::string name);

		Shader& CreateShader(std::string name);

		//Mesh& GetObjectByName(std::string name);
		Shader& GetShaderByName(std::string name);
		//Camera& GetCamera();

	};
}