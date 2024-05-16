#pragma once
#include "merlin/graphics/rendererBase.h"
#include "merlin/scene/scene.h"
#include "merlin/scene/camera.h"
#include "merlin/physics/particleSystem.h"
#include <stack>

namespace Merlin {
	class Renderer : public RendererBase{
	public:
		Renderer();
		~Renderer();

		void clear() override;

		void renderLight(const Light& li, const Camera& camera);
		void renderMesh(const Mesh& mesh, const Camera& camera);
		void renderModel(const Model& model, const Camera& camera);
		//void renderParticleSystem(const ParticleSystem& ps, const Camera& camera);
		void renderTransformObject(const TransformObject& obj, const Camera& camera);

		void renderScene(const Scene& model, const Camera& camera);
		void render(const Shared<RenderableObject>& object, const Camera& camera);
		
		void initialize();		

		void loadShader(const std::string& name, const std::string& vertexShaderPath, const std::string& fragmentShaderPath, const std::string& geomShaderPath = "");

		void addMaterial(Shared<MaterialBase> material);
		void addShader(Shared<Shader> shader );

		Shared<Shader> getShader(std::string n);

	private:
		

		void pushMatrix();
		void popMatrix();
		void resetMatrix();

		glm::mat4 currentTransform;
		std::stack<glm::mat4> matrixStack;
		std::vector<Shared<Light>> m_activeLights;

		Shared<Environment> m_defaultEnvironment = nullptr;
		Shared<Environment> m_currentEnvironment = nullptr;

		MaterialLibrary* m_materialLibrary = nullptr;
		ShaderLibrary* m_shaderLibrary = nullptr;
	};
}