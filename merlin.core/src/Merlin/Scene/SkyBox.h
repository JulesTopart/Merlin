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
		inline void SetCubeMap(const Shared<CubeMap> cubemap) { _cubeMap = cubemap; }

		inline bool HasShader() const { return _shader != nullptr; }
		inline const Shader& GetShader() const { return *_shader; }
		inline void SetShader(Shared<Shader> shader) { _shader = shader; }
		inline const std::string& GetShaderName() const { return _shaderName; }
		inline void SetShader(std::string shaderName) { _shaderName = shaderName; }

		static Shared<SkyBox>Create(std::string name, std::vector<std::string> paths);

	private:
		Scope<VAO> _vao;
		Shared<CubeMap> _cubeMap;
		std::string _shaderName = "default";
		Shared<Shader> _shader;

	};

}
