#pragma once
#include "merlin/core/core.h"
#include "merlin/memory/cubeMap.h"
#include "merlin/graphics/renderableObject.h"

namespace Merlin {

	class SkyBox : public RenderableObject {
	public:
		SkyBox(std::string name);
		SkyBox(std::string name, std::vector<std::string> paths);
		~SkyBox() {};

		void draw() const;

		void loadCubeMap(std::vector<std::string> paths);
		inline void setCubeMap(const Shared<CubeMap> cubemap) { m_cubeMap = cubemap; }

		inline bool hasShader() const { return m_shader != nullptr; }
		inline bool hasCubeMap() const { return m_cubeMap != nullptr; }
		inline const Shader& getShader() const { return *m_shader; }
		inline void setShader(Shared<Shader> shader) { m_shader = shader; }
		inline const std::string& getShaderName() const { return m_shaderName; }
		inline void setShader(std::string shaderName) { m_shaderName = shaderName; }

		static Shared<SkyBox>create(std::string name);
		static Shared<SkyBox>create(std::string name, std::vector<std::string> paths);

	private:
		VAO m_vao;
		Shared<CubeMap> m_cubeMap;
		std::string m_shaderName = "default";
		Shared<Shader> m_shader;

	};
	typedef Shared<SkyBox> SkyBox_Ptr;
}
