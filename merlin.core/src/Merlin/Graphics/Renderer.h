#pragma once
#include "Merlin/Graphics/RendererBase.h"
#include "Merlin/Scene/Scene.h"
#include "Merlin/Scene/Camera.h"
#include "Merlin/Physics/ParticleSystem.h"
#include <stack>

namespace Merlin {
	class Renderer : public RendererBase{
	public:
		Renderer();
		~Renderer();

		void clear() override;

		void renderSkyBox(const SkyBox& sky, const Camera& camera);
		void renderMesh(const Mesh& mesh, const Camera& camera);
		void renderModel(const Model& model, const Camera& camera);
		void renderParticleSystem(const ParticleSystem& ps, const Camera& camera);
		void deprecated_renderParticleSystem(const deprecated_GenericParticleSystem& ps, const Camera& camera);
		void renderTransformObject(const TransformObject& obj, const Camera& camera);

		void renderScene(const Scene& model, const Camera& camera);
		void render(const Shared<RenderableObject>& object, const Camera& camera);
		
		void initialize();		

		void loadShader(const std::string& name, const std::string& vertexShaderPath, const std::string& fragmentShaderPath, const std::string& geomShaderPath = "");
		void createMaterial(MaterialProperty matProps);
		void addMaterial(Shared<Material> material);
		void addShader(Shared<Shader> shader );

		const Material& getMaterial(std::string n);
		const Shader& getShader(std::string n);
		const Shared<Shader>& shareShader(std::string n);

	private:
		

		void pushMatrix();
		void popMatrix();
		void resetMatrix();

		glm::mat4 currentTransform;
		std::stack<glm::mat4> matrixStack;

		ShaderLibrary m_shaderLibrary;
		MaterialLibrary m_materialLibrary;
	};
}
