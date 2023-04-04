#pragma once
#include "Merlin/Scene/Scene.h"
#include "Merlin/Scene/Camera.h"


namespace Merlin::Graphics {
	class Renderer{
	public:
		Renderer();
		~Renderer();

		void Render(const Model& model, const Camera& camera);
		void Render(const Mesh& mesh, const Camera& camera);

		void Initialize();
		void Clear();
		void SetBackgroundColor(float r, float g, float b, float a);
		
		void EnableTransparency();
		void EnableMultisampling();
		void DisableTransparency();
		void DisableMultisampling();

		void LoadShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath, const std::string& geomShaderPath = "");
		void CreateMaterial(MaterialProperty matProps);
		void AddMaterial(Shared<Material> material);
		void AddShader(Shared<Shader> shader );

		const Material& GetMaterial(std::string n);
		const Shader& GetShader(std::string n);

	private:
		ShaderLibrary _shaderLibrary;
		MaterialLibrary _materialLibrary;

		glm::vec4 backgroundColor;
	};
}
