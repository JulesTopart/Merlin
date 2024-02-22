#pragma once
#include "Merlin/Graphics/RendererBase.h"
#include "Merlin/Scene/Scene.h"
#include "Merlin/Scene/Camera.h"
#include <stack>

namespace Merlin {
	class Renderer : public RendererBase{
	public:
		Renderer();
		~Renderer();

		void Clear() override;

		void RenderSkyBox(const SkyBox& sky, const Camera& camera);
		void RenderMesh(const Mesh& mesh, const Camera& camera);
		void RenderModel(const Model& model, const Camera& camera);
		void RenderParticleSystem(const deprecated_GenericParticleSystem& ps, const Camera& camera);
		void RenderTransformObject(const TransformObject& obj, const Camera& camera);

		void RenderScene(const Scene& model, const Camera& camera);
		void Render(const Shared<RenderableObject>& object, const Camera& camera);
		
		void Initialize();		

		void LoadShader(const std::string& name, const std::string& vertexShaderPath, const std::string& fragmentShaderPath, const std::string& geomShaderPath = "");
		void CreateMaterial(MaterialProperty matProps);
		void AddMaterial(Shared<Material> material);
		void AddShader(Shared<Shader> shader );

		const Material& GetMaterial(std::string n);
		const Shader& GetShader(std::string n);
		const Shared<Shader>& ShareShader(std::string n);

	private:
		

		void PushMatrix();
		void PopMatrix();
		void ResetMatrix();

		glm::mat4 currentTransform;
		std::stack<glm::mat4> matrixStack;

		ShaderLibrary m_shaderLibrary;
		MaterialLibrary m_materialLibrary;
	};
}
