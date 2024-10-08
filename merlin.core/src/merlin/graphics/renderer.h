#pragma once
#include "merlin/graphics/rendererBase.h"
#include "merlin/scene/scene.h"
#include "merlin/scene/camera.h"
#include "merlin/physics/particleSystem.h"
#include "merlin/physics/isoSurface.h"
#include "merlin/memory/frameBuffer.h"
#include <stack>

namespace Merlin {
	class Renderer : public RendererBase {
	public:
		Renderer();
		~Renderer();

		void clear() override;

		void castShadow(Shared<Light> light, const Scene& scene);
		void gatherLights(const Shared<RenderableObject>& object);


		void renderDepth(const Shared<RenderableObject>& object, Shared<Shader> depthShader);
		void renderLight(const Light& li, const Camera& camera);
		void renderMesh(const Mesh& mesh, const Camera& camera);
		void renderEnvironment(const Environment& env, const Camera& camera);
		void renderParticleSystem(const ParticleSystem& ps, const Camera& camera);
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

		inline void useDefaultLight(bool state) { use_default_light = state; }
		inline bool useDefaultLight() { return use_default_light; }

		inline void enableShadows() { use_shadows = true; }
		inline void disableShadows() { use_shadows = false; }

		Shared<Shader> getShader(std::string n);
		Shared<MaterialBase> getMaterial(std::string n);

		inline void resetGlobalTransform() { m_globalTransform = glm::mat4(1); }
		inline void applyGlobalTransform(glm::mat4 globalTransform) {m_globalTransform = globalTransform;}

	private:
		bool debug = false;
		bool use_shadows = true;
		bool use_environment = true;
		bool display_lights = false;
		bool use_default_light = false; //Use has backup if no light set

		Shared<AmbientLight> m_defaultAmbient;
		Shared<DirectionalLight> m_defaultDirLight;
		Shared<DirectionalLight> m_defaultDirLight2;
		Shared<DirectionalLight> m_defaultDirLight3;

		void pushMatrix();
		void popMatrix();
		void resetMatrix();
		
		float m_scene_scale = 1.0;

		glm::mat4 m_globalTransform = glm::mat4(1);
		glm::mat4 currentTransform;
		std::stack<glm::mat4> matrixStack;
		std::vector<Shared<Light>> m_activeLights;

		Shared<Environment> m_defaultEnvironment = nullptr;
		Shared<Environment> m_currentEnvironment = nullptr;
	};
}
