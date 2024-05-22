#pragma once
#include "merlin/graphics/rendererBase.h"
#include "merlin/scene/scene.h"
#include "merlin/scene/camera.h"
#include "merlin/physics/particleSystem.h"
#include "merlin/memory/frameBuffer.h"
#include <stack>

namespace Merlin {
	class Renderer : public RendererBase {
	public:
		Renderer();
		~Renderer();

		void clear() override;

		void castShadow(Shared<Light> light, const Scene& scene);

		void renderDepth(const Shared<RenderableObject>& object, Shared<Shader> depthShader);
		void renderLight(const Light& li, const Camera& camera);
		void renderMesh(const Mesh& mesh, const Camera& camera);
		void renderEnvironment(const Environment& env, const Camera& camera);
		//void renderParticleSystem(const ParticleSystem& ps, const Camera& camera);
		void renderTransformObject(const TransformObject& obj, const Camera& camera);

		void renderScene(const Scene& model, const Camera& camera);
		void render(const Shared<RenderableObject>& object, const Camera& camera);

		void initialize();

		void loadShader(const std::string& name, const std::string& vertexShaderPath, const std::string& fragmentShaderPath, const std::string& geomShaderPath = "");

		void addMaterial(Shared<MaterialBase> material);
		void addShader(Shared<Shader> shader);

		inline void disableEnvironment() { use_environment = false; }
		inline void enableEnvironment(){use_environment = true;}
		void setEnvironmentGradientColor(float r, float g, float b);
		void setEnvironmentGradientColor(glm::vec3 color);

		inline void hideLights() { display_lights = false; }
		inline void showLights() { display_lights = true; }

		inline void enableShadows() { use_shadows = true; }
		inline void disableShadows() { use_shadows = false; }

		Shared<Shader> getShader(std::string n);
		Shared<MaterialBase> getMaterial(std::string n);

	private:
		bool use_shadows = true;
		bool use_environment = true;

		bool display_lights = false;
		void pushMatrix();
		void popMatrix();
		void resetMatrix();

		glm::mat4 currentTransform;
		std::stack<glm::mat4> matrixStack;
		std::vector<Shared<Light>> m_activeLights;

		Shared<Environment> m_defaultEnvironment = nullptr;
		Shared<Environment> m_currentEnvironment = nullptr;
	};
}
