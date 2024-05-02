#pragma once
#include "merlin/core/core.h"
#include "merlin/memory/Texture.h"
#include "merlin/graphics/renderableObject.h"

namespace Merlin {

	class Environment : public RenderableObject {
	public:
		Environment(std::string name) : RenderableObject(name) {}
		Shared<Texture> hdrTexture;
		Shared<Texture> irradianceMap;
		Shared<Texture> reflectionMap;

		// Setup methods for different textures
		void loadHDR(const std::string& path);

		inline bool hasShader() const { return m_shader != nullptr; }
		inline const Shader& getShader() const { return *m_shader; }
		inline void setShader(Shared<Shader> shader) { m_shader = shader; }
		inline const std::string& getShaderName() const { return m_shaderName; }
		inline void setShader(std::string shaderName) { m_shaderName = shaderName; }

	private:
		void setupMesh();

		VAO m_vao;
		std::string m_shaderName = "default";
		Shared<Shader> m_shader;

	};
	typedef Shared<Environment> Environment_Ptr;
}
