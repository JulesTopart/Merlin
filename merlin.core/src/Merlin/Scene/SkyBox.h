#pragma once
#include "Merlin/Core/Core.h"
#include "Merlin/Memory/CubeMap.h"
#include "Merlin/Graphics/RenderableObject.h"



using namespace Merlin::Memory;

namespace Merlin::Graphics {

	class SkyBox : public RenderableObject {
	public:
		SkyBox(std::string name);
		SkyBox(std::string name, std::vector<std::string> paths);
		~SkyBox() {};

		void Draw() const;

		void LoadCubeMap(std::vector<std::string> paths);
		inline void SetCubeMap(const Shared<CubeMap> cubemap) { m_cubeMap = cubemap; }

		inline bool HasShader() const { return m_shader != nullptr; }
		inline bool HasCubeMap() const { return m_cubeMap != nullptr; }
		inline const Shader& GetShader() const { return *m_shader; }
		inline void SetShader(Shared<Shader> shader) { m_shader = shader; }
		inline const std::string& GetShaderName() const { return m_shaderName; }
		inline void SetShader(std::string shaderName) { m_shaderName = shaderName; }

		static Shared<SkyBox>Create(std::string name);
		static Shared<SkyBox>Create(std::string name, std::vector<std::string> paths);

	private:
		VAO m_vao;
		Shared<CubeMap> m_cubeMap;
		std::string m_shaderName = "default";
		Shared<Shader> m_shader;

	};
	typedef Shared<SkyBox> SkyBox_Ptr;
}
