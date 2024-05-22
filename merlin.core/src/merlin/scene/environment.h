#pragma once
#include "merlin/core/core.h"
#include "merlin/memory/Texture.h"
#include "merlin/memory/CubeMap.h"
#include "merlin/graphics/renderableObject.h"

namespace Merlin {

	class Environment : public RenderableObject {
	public:
		Environment(std::string name, GLuint resolution = 512);

		

		// Setup methods for different textures
		//void loadHDR(const std::string& path);
		void detach() const;
		void attach(Shader& shader) const;
		void draw() const;

		inline bool hasShader() const { return m_shader != nullptr; }
		inline const Shader& getShader() const { return *m_shader; }
		inline void setShader(Shared<Shader> shader) { m_shader = shader; }
		inline const std::string& getShaderName() const { return m_shaderName; }
		inline void setShader(std::string shaderName) { m_shaderName = shaderName; }

		inline void setGradientColor(const glm::vec3& c) { m_gradient_color = c; }
		const glm::vec3& gradientColor() const { return m_gradient_color; }

		inline void setCubeMap(Shared<CubeMap> cm) { m_skybox = cm; }

	private:
		void setupMesh();

		VAO m_vao;
		std::string m_shaderName = "default";
		Shared<Shader> m_shader;
		Shared<Texture2D> raw_hdri = nullptr;

		GLuint m_resolution = 512;

		glm::vec3 m_gradient_color = glm::vec3(1);
		Shared<CubeMap> m_skybox = nullptr;
		Shared<CubeMap> m_irradiance = nullptr;

		inline static Shared<Shader> conversionShader = nullptr;
		inline static Shared<Shader> convolutionShader = nullptr;
	};
	typedef Shared<Environment> Environment_Ptr;
}
